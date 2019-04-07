#ifndef MPDEVICEBLEIMPL_H
#define MPDEVICEBLEIMPL_H

#include "MPDevice.h"
#include "BleCommon.h"

class MessageProtocolBLE;

/**
 * @brief The MPDeviceBleImpl class
 * Implementations of only BLE related commands
 * and helper functions
 */
class MPDeviceBleImpl: public QObject
{
    Q_OBJECT

    QT_WRITABLE_PROPERTY(QString, mainMCUVersion, QString())
    QT_WRITABLE_PROPERTY(QString, auxMCUVersion, QString())

public:
    MPDeviceBleImpl(MessageProtocolBLE *mesProt, MPDevice *dev);

    bool isFirstPacket(const QByteArray &data);
    bool isLastPacket(const QByteArray &data);

    void getPlatInfo();
    void getDebugPlatInfo(const MessageHandlerCbData &cb);
    QVector<int> calcDebugPlatInfo(const QByteArray &platInfo);

    void flashMCU(QString type, const MessageHandlerCb &cb);
    void uploadBundle(QString filePath, const MessageHandlerCb &cb, const MPDeviceProgressCb &cbProgress);
    void fetchData(QString filePath, MPCmd::Command cmd);
    inline void stopFetchData() { fetchState = Common::FetchState::STOPPED; }

    void sendResetFlipBit();
    void flipMessageBit(QByteArray &msg);

    /**
     * @brief storeCredential
     * Only for testing without the callback
     */
    //TODO: Only for testing
    void storeCredential(const BleCredential &cred);
    void storeCredential(const BleCredential &cred, MessageHandlerCb cb);
    /**
     * @brief getCredential
     * Only for testing without the callback
     */
    //TODO: Only for testing
    void getCredential(QString service, QString login = "");
    void getCredential(QString service, QString login, const MessageHandlerCbData &cb);
    BleCredential retrieveCredentialFromResponse(QByteArray response, QString service, QString login) const;

private:
    void checkDataFlash(const QByteArray &data, QElapsedTimer *timer, AsyncJobs *jobs, QString filePath, const MPDeviceProgressCb &cbProgress);
    void sendBundleToDevice(QString filePath, AsyncJobs *jobs, const MPDeviceProgressCb &cbProgress);
    void writeFetchData(QFile *file, MPCmd::Command cmd);

    QByteArray createStoreCredMessage(const BleCredential &cred);
    QByteArray createGetCredMessage(QString service, QString login);
    QByteArray createCredentialMessage(const CredMap &credMap);

    void dequeueAndRun(AsyncJobs *job);


    MessageProtocolBLE *bleProt;
    MPDevice *mpDev;
    Common::FetchState fetchState = Common::FetchState::STOPPED;

    static constexpr int BUNBLE_DATA_WRITE_SIZE = 256;
    static constexpr int BUNBLE_DATA_ADDRESS_SIZE = 4;
};

#endif // MPDEVICEBLEIMPL_H
