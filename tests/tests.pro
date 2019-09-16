#-------------------------------------------------
#
# Project created by QtCreator 2017-08-01T23:36:26
#
#-------------------------------------------------

QT       += testlib websockets

QT       -= gui

QMAKE_CXXFLAGS += -std=c++0x

TARGET = tests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include (../src/QSimpleUpdater/QSimpleUpdater.pri)

INCLUDEPATH += $$PWD/../src $$PWD/../src/MessageProtocol $$PWD/../src/Mooltipass $$PWD/../src/Settings

win32: QT += core-private

win32 {
    LIBS += -lsetupapi -luser32
} else:linux {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += libudev
    QT -= widgets
} else:mac {
    LIBS += -framework ApplicationServices -framework IOKit -framework CoreFoundation -framework Cocoa -framework Foundation
}

win32 {
    #Private header qwinoverlappedionotifier_p.h removed from Qt5.10 SDK
    #so need to include files explicitly.
    greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 9) {
        SOURCES += ../src/qwinoverlappedionotifier.cpp
        HEADERS += ../src/qwinoverlappedionotifier.h
    }
    SOURCES += ../src/UsbMonitor_win.cpp \
               ../src/MPDevice_win.cpp \
               ../src/HIDLoader.cpp
    HEADERS += ../src/UsbMonitor_win.h \
               ../src/MPDevice_win.h \
               ../src/HIDLoader.h \
               ../src/hid_dll.h
}
linux {
    SOURCES += ../src/UsbMonitor_linux.cpp \
               ../src/MPDevice_linux.cpp
    HEADERS += ../src/UsbMonitor_linux.h \
               ../src/MPDevice_linux.h
}
mac {
    SOURCES += ../src/UsbMonitor_mac.cpp \
               ../src/MPDevice_mac.cpp
    HEADERS += ../src/UsbMonitor_mac.h \
               ../src/MPDevice_mac.h

    HEADERS += ../src/MacUtils.h
    SOURCES += ../src/MacUtils.mm
}

SOURCES += \
    ../src/MPDevice.cpp \
    ../src/MPManager.cpp \
    ../src/Common.cpp \
    ../src/WSServer.cpp \
    ../src/AppDaemon.cpp \
    ../src/AsyncJobs.cpp \
    ../src/Mooltipass/MPNode.cpp \
    ../src/WSServerCon.cpp \
    ../src/MPDevice_emul.cpp \
    ../src/http-parser/http_parser.c \
    ../src/HttpClient.cpp \
    ../src/HttpServer.cpp \
    ../src/MooltipassCmds.cpp \
    ../src/MessageProtocol/MessageProtocolMini.cpp \
    ../src/MessageProtocol/MessageProtocolBLE.cpp \
    ../src/MPDeviceBleImpl.cpp \
    ../src/HaveIBeenPwned.cpp \
    ../src/Mooltipass/MPNodeMini.cpp \
    ../src/Mooltipass/MPNodeBLE.cpp \
    ../src/Mooltipass/MPSettingsMini.cpp \
    ../src/Mooltipass/MPSettingsBLE.cpp \
    ../src/Settings/DeviceSettings.cpp \
    ../src/Settings/DeviceSettingsMini.cpp \
    ../src/Settings/DeviceSettingsBLE.cpp \
    ../src/Mooltipass/MPBLEFreeAddressProvider.cpp \
    ../src/SimpleCrypt/SimpleCrypt.cpp \
    ../src/FilesCache.cpp \
    ../src/DbBackupsTracker.cpp \
    ../src/TreeItem.cpp \
    ../src/RootItem.cpp \
    ../src/LoginItem.cpp \
    ../src/ServiceItem.cpp \
    ../src/CredentialModel.cpp \
    ../src/CredentialModelFilter.cpp \
    ../src/DbExportsRegistry.cpp \
    ../src/DbBackupChangeNumbersComparator.cpp \
    ../src/ParseDomain.cpp \
    main.cpp \
    FilesCacheTests.cpp \
    UpdaterTests.cpp \
    DbBackupsTrackerTests.cpp \
    TestTreeItem.cpp \
    TestCredentialModel.cpp \
    TestCredentialModelFilter.cpp \
    TestDbExportsRegistry.cpp \
    TestParseDomain.cpp \
    TestGetFreeAddresses.cpp

HEADERS += \
    ../src/SimpleCrypt/SimpleCrypt.h \
    ../src/DbBackupsTracker.h\
    ../src/TreeItem.h \
    ../src/RootItem.h \
    ../src/LoginItem.h \
    ../src/ServiceItem.h \
    ../src/CredentialModel.h \
    ../src/CredentialModelFilter.h \
    ../src/DbExportsRegistry.h \
    ../src/DbBackupChangeNumbersComparator.h \
    ../src/Common.h \
    ../src/MPDevice.h \
    ../src/MPManager.h \
    ../src/MooltipassCmds.h \
    ../src/QtHelper.h \
    ../src/WSServer.h \
    ../src/AppDaemon.h \
    ../src/AsyncJobs.h \
    ../src/Mooltipass/MPNode.h \
    ../src/version.h \
    ../src/WSServerCon.h \
    ../src/MPDevice_emul.h \
    ../src/http-parser/http_parser.h \
    ../src/HttpClient.h \
    ../src/HttpServer.h \
    ../src/FilesCache.h \
    ../src/ParseDomain.h \
    ../src/MessageProtocol/IMessageProtocol.h \
    ../src/MessageProtocol/MessageProtocolMini.h \
    ../src/MessageProtocol/MessageProtocolBLE.h \
    ../src/MPDeviceBleImpl.h \
    ../src/HaveIBeenPwned.h \
    ../src/BleCommon.h \
    ../src/Mooltipass/MPNodeMini.h \
    ../src/Mooltipass/MPNodeBLE.h \
    ../src/Mooltipass/MPSettingsMini.h \
    ../src/Mooltipass/MPSettingsBLE.h \
    ../src/Settings/DeviceSettings.h \
    ../src/Settings/DeviceSettingsMini.h \
    ../src/Settings/DeviceSettingsBLE.h \
    ../src/Mooltipass/MPBLEFreeAddressProvider.h \
    UpdaterTests.h \
    FilesCacheTests.h \
    DbBackupsTrackerTests.h \
    TestTreeItem.h \
    TestCredentialModel.h \
    TestCredentialModelFilter.h \
    TestDbExportsRegistry.h \
    TestParseDomain.h \
    TestGetFreeAddresses.h

DEFINES += SRCDIR=\\\"$$PWD/\\\"
