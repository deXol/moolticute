#ifndef TESTGETFREEADDRESSES_H
#define TESTGETFREEADDRESSES_H

#include <QObject>
#include "MPBLEFreeAddressProvider.h"
#include "MessageProtocolBLE.h"


class TestGetFreeAddresses : public QObject
{
    Q_OBJECT
public:
    explicit TestGetFreeAddresses(QObject *parent = nullptr);

private Q_SLOTS:
    void testGetFreeAddr();

private:
    MPDevice *mpDev;
    MessageProtocolBLE bleProt = MessageProtocolBLE();
    MPBLEFreeAddressProvider freeaddressProv;
};

#endif // TESTGETFREEADDRESSES_H
