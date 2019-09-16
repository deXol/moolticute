#include "TestGetFreeAddresses.h"
#include "MPDevice_emul.h"
#include <QSignalSpy>

TestGetFreeAddresses::TestGetFreeAddresses(QObject *parent)
    : QObject(parent),
      mpDev{new MPDevice_emul(this)},
      freeaddressProv{MPBLEFreeAddressProvider(&bleProt, mpDev)}
{
}

void TestGetFreeAddresses::testGetFreeAddr()
{
    freeaddressProv.incrementChildNodeNeeded(1);
    freeaddressProv.incrementChildNodeNeeded(2);
    AsyncJobs *jobs = new AsyncJobs("Test Free Address", this);
    freeaddressProv.loadFreeAddresses(jobs, QByteArray{0}, [](const QVariantMap &){});
    connect(jobs, &AsyncJobs::finished, [](const QByteArray &)
    {
        qDebug() << "asyncFinished";
        Q_ASSERT(true);
    });

    connect(jobs, &AsyncJobs::failed, [](AsyncJob *)
    {
        qDebug() << "asyncFailed";
        Q_ASSERT(false);
    });
    jobs->start();

//    QSignalSpy spy{jobs, SIGNAL(failed(AsyncJob *))};
//    spy.wait();

    Q_ASSERT(true);
}
