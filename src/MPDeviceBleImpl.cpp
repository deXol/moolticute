#include "MPDeviceBleImpl.h"
#include "AsyncJobs.h"

MPDeviceBleImpl::MPDeviceBleImpl(MessageProtocolBLE* mesProt, MPDevice *dev):
    bleProt(mesProt),
    mpDev(dev)
{

}

void MPDeviceBleImpl::getPlatInfo(const MessageHandlerCb &cb)
{
    auto *jobs = new AsyncJobs("Get PlatInfo", mpDev);

    AsyncFuncDone processPlatformInfo = [this](const QByteArray &data, bool &done) -> bool
    {
        quint8 total = data[1] & 0x0F;
        quint8 act = (data[1] & 0xF0) >> 4;
        if (0 == act)
        {
            platInfo.clear();
        }
#ifdef DEV_DEBUG
        qDebug() << "Actual packet: " << act << ", total packets: " << total;
#endif
        platInfo.append(bleProt->getFullPayload(data));
        if (act != total)
        {
            done = false;
        }
#ifdef DEV_DEBUG
        else
        {
            qDebug() << "ALL packets received!";
        }
#endif
        return true;
    };

    jobs->append(new MPCommandJob(mpDev, MPCmd::CMD_DBG_GET_PLAT_INFO, processPlatformInfo));

    connect(jobs, &AsyncJobs::finished, [cb](const QByteArray &data)
    {
        Q_UNUSED(data);
        /* Callback */
        cb(true, "");
    });

    dequeueAndRun(jobs);
}

QVector<int> MPDeviceBleImpl::calcPlatInfo()
{
    QVector<int> platInfos;
    platInfos.append(bleProt->toIntFromBigEndian(static_cast<quint8>(platInfo[0]), static_cast<quint8>(platInfo[1])));
    platInfos.append(bleProt->toIntFromBigEndian(static_cast<quint8>(platInfo[2]), static_cast<quint8>(platInfo[3])));
    platInfos.append(bleProt->toIntFromBigEndian(static_cast<quint8>(platInfo[64]), static_cast<quint8>(platInfo[65])));
    platInfos.append(bleProt->toIntFromBigEndian(static_cast<quint8>(platInfo[66]), static_cast<quint8>(platInfo[67])));
    return platInfos;
}

void MPDeviceBleImpl::flashMCU(QString type, const MessageHandlerCb &cb)
{
    if (type != "aux" && type != "main")
    {
        qCritical() << "Flashing called for an invalid type of " << type;
        cb(false, "Failed flash.");
        return;
    }
    auto *jobs = new AsyncJobs(QString("Flashing %1 MCU").arg(type), this);
    const bool isAuxFlash = type == "aux";
    const quint8 cmd = isAuxFlash ? MPCmd::CMD_DBG_FLASH_AUX_MCU : MPCmd::CMD_DBG_REBOOT_TO_BOOTLOADER;
    jobs->append(new MPCommandJob(mpDev, cmd, bleProt->getDefaultFuncDone()));

    connect(jobs, &AsyncJobs::failed, [cb, isAuxFlash](AsyncJob *failedJob)
    {
        Q_UNUSED(failedJob);
        if (isAuxFlash)
        {
            cb(false, "Failed to flash Aux MCU!");
        }
        else
        {
            /**
              * Main MCU flash does not cause a reconnect of the device,
              * so daemon keeps sending messages to the device, which
              * is causing failure in the communication, this is why
              * this sleep is added.
              */
            qDebug() << "Waiting for device to start after Main MCU flash";
            QThread::sleep(5);
            cb(true, "");
        }
    });

    dequeueAndRun(jobs);
}

void MPDeviceBleImpl::uploadBundle(QString filePath, const MessageHandlerCb &cb)
{
    QElapsedTimer *timer = new QElapsedTimer{};
    timer->start();
    auto *jobs = new AsyncJobs(QString("Upload bundle file"), this);
    jobs->append(new MPCommandJob(mpDev, MPCmd::CMD_DBG_ERASE_DATA_FLASH, bleProt->getDefaultFuncDone()));

    jobs->append(new MPCommandJob(mpDev, MPCmd::CMD_DBG_IS_DATA_FLASH_READY,
                    [this, timer, jobs, filePath](const QByteArray &data, bool &) -> bool
                    {
                        checkDataFlash(data, timer, jobs, filePath);
                        return true;
                    }));

    connect(jobs, &AsyncJobs::failed, [cb](AsyncJob *failedJob)
    {
        Q_UNUSED(failedJob);
        cb(false, "Erase Data failed");
    });

    connect(jobs, &AsyncJobs::finished, [cb](const QByteArray &)
    {
        cb(true, "");
    });

    dequeueAndRun(jobs);
}

void MPDeviceBleImpl::checkDataFlash(const QByteArray &data, QElapsedTimer *timer, AsyncJobs *jobs, QString filePath)
{
    if (0x01 == bleProt->getFirstPayloadByte(data))
    {
        qDebug() << "Erase done in: " << timer->nsecsElapsed() / 1000000 << " ms";
        delete timer;
        sendBundleToDevice(filePath, jobs);
    }
    else
    {
        //Erase is not done yet.
        jobs->prepend(new MPCommandJob(mpDev, MPCmd::CMD_DBG_IS_DATA_FLASH_READY,
                       [this, timer, jobs, filePath](const QByteArray &data, bool &) -> bool
                       {
                           checkDataFlash(data, timer, jobs, filePath);
                           return true;
                       }));
    }
}

void MPDeviceBleImpl::sendBundleToDevice(QString filePath, AsyncJobs *jobs)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical() << "Error opening bundle file: " << filePath;
        return;
    }
    QByteArray blob = file.readAll();
    qDebug() << "Bundle size: " << blob.size();
    int byteCounter = 4;
    int curAddress = 0;
    QByteArray message;
    message.fill(static_cast<char>(0), 4);
    for (const auto byte : blob)
    {
        message.append(byte);
        ++byteCounter;
        if (260 == byteCounter)
        {
            jobs->append(new MPCommandJob(mpDev, MPCmd::CMD_DBG_DATAFLASH_WRITE_256B, message,
                              [curAddress](const QByteArray &data, bool &) -> bool
                                  {
                                      Q_UNUSED(data);
#ifdef DEV_DEBUG
                                      qDebug() << "Sending message to address #" << curAddress;
#endif
                                      return true;
                                  }));
            curAddress += 256;
            message.clear();
            quint32 qCurAddress = static_cast<quint32>(curAddress);
            //Add write address to message (Big endian)
            message.append((qCurAddress&0xFF));
            message.append(((qCurAddress&0xFF00)>>8));
            message.append(((qCurAddress&0xFF0000)>>16));
            message.append(((qCurAddress&0xFF000000)>>24));
            byteCounter = 4;
        }
    }
    jobs->append(new MPCommandJob(mpDev, MPCmd::CMD_DBG_DATAFLASH_WRITE_256B, message,
                  [](const QByteArray &data, bool &) -> bool
                      {
                          Q_UNUSED(data);
                          qDebug() << "Sending bundle is DONE";
                          return true;
                      }));

    jobs->append(new MPCommandJob(mpDev, MPCmd::CMD_DBG_REINDEX_BUNDLE, bleProt->getDefaultFuncDone()));
}

void MPDeviceBleImpl::dequeueAndRun(AsyncJobs *jobs)
{
    mpDev->jobsQueue.enqueue(jobs);
    mpDev->runAndDequeueJobs();
}