QT       += core network websockets gui widgets

TEMPLATE = app

TARGET = moolticute

CONFIG += c++11

INCLUDEPATH += $$PWD/src $$PWD/src/Settings $$PWD/src/utils

mac {
    LIBS += -framework ApplicationServices -framework IOKit -framework CoreFoundation -framework Cocoa -framework Foundation
}
win32 {
    LIBS += -luser32
}
linux {
    QT += dbus
}

include(src/QtAwesome/QtAwesome/QtAwesome.pri)
include(src/QSimpleUpdater/QSimpleUpdater.pri)

CONFIG(use_system_qzxing) {
    LIBS += -lQZXing
} else {
    include(src/QZXing/src/QZXing.pri)
}

greaterThan(QT_MAJOR_VERSION, 5) {
    include (src/qtcsv6/qtcsv.pri)
} else {
    include (src/qtcsv/qtcsv.pri)
}

SOURCES += src/main_gui.cpp \
    src/ClickableLabel.cpp \
    src/utils/DeviceConnectionChecker.cpp \
    src/MainWindow.cpp \
    src/NoScrollComboBox.cpp \
    src/NotesManagement.cpp \
    src/ParseDomain.cpp \
    src/Common.cpp \
    src/TOTPCredential.cpp \
    src/TutorialWidget.cpp \
    src/WSClient.cpp \
    src/RotateSpinner.cpp \
    src/AppGui.cpp \
    src/DaemonMenuAction.cpp \
    src/AutoStartup.cpp \
    src/WindowLog.cpp \
    src/OutputLog.cpp \
    src/AnsiEscapeCodeHandler/AnsiEscapeCodeHandler.cpp \
    src/PasswordLineEdit.cpp \
    src/CredentialsManagement.cpp \
    src/utils/GridLayoutUtil.cpp \
    src/utils/KeyboardLayoutDetector.cpp \
    src/utils/TOTPReader.cpp \
    src/zxcvbn-c/zxcvbn.c \
    src/FilesManagement.cpp \
    src/SSHManagement.cpp \
    src/CredentialView.cpp \
    src/CredentialModel.cpp \
    src/CredentialModelFilter.cpp \
    src/ItemDelegate.cpp \
    src/LoginItem.cpp \
    src/TreeItem.cpp \
    src/ServiceItem.cpp \
    src/RootItem.cpp \
    src/AnimatedColorButton.cpp \
    src/PasswordProfilesModel.cpp \
    src/PassGenerationProfilesDialog.cpp \
    src/DbBackupsTracker.cpp \
    src/DbBackupsTrackerController.cpp \
    src/PromptWidget.cpp \
    src/DbExportsRegistry.cpp \
    src/DbExportsRegistryController.cpp \
    src/DbBackupChangeNumbersComparator.cpp \
    src/DbMasterController.cpp \
    src/SystemEventHandler.cpp \
    src/SystemNotifications/SystemNotification.cpp \
    src/RequestLoginNameDialog.cpp \
    src/RequestDomainSelectionDialog.cpp \
    src/BleDev.cpp \
    src/Settings/DeviceSettings.cpp \
    src/Settings/DeviceSettingsBLE.cpp \
    src/Settings/DeviceSettingsMini.cpp \
    src/Settings/SettingsGuiHelper.cpp \
    src/Settings/SettingsGuiMini.cpp \
    src/Settings/SettingsGuiBLE.cpp \
    src/DeviceDetector.cpp \
    src/FidoManagement.cpp \

HEADERS  += src/MainWindow.h \
    src/ClickableLabel.h \
    src/utils/DeviceConnectionChecker.h \
    src/NoScrollComboBox.h \
    src/NotesManagement.h \
    src/ParseDomain.h \
    src/Common.h \
    src/QtHelper.h \
    src/TOTPCredential.h \
    src/TutorialWidget.h \
    src/WSClient.h \
    src/RotateSpinner.h \
    src/utils/GridLayoutUtil.h \
    src/utils/KeyboardLayoutDetector.h \
    src/utils/TOTPReader.h \
    src/utils/qurltlds_p.h \
    src/version.h \
    src/AppGui.h \
    src/DaemonMenuAction.h \
    src/AutoStartup.h \
    src/WindowLog.h \
    src/OutputLog.h \
    src/AnsiEscapeCodeHandler/AnsiEscapeCodeHandler.h \
    src/PasswordLineEdit.h \
    src/CredentialsManagement.h \
    src/zxcvbn-c/dict-src.h \
    src/zxcvbn-c/zxcvbn.h \
    src/FilesManagement.h \
    src/SSHManagement.h \
    src/CredentialView.h \
    src/ServiceItem.h \
    src/TreeItem.h \
    src/RootItem.h \
    src/LoginItem.h \
    src/ItemDelegate.h \
    src/CredentialModel.h \
    src/CredentialModelFilter.h \
    src/AnimatedColorButton.h \
    src/PasswordProfilesModel.h \
    src/PassGenerationProfilesDialog.h \
    src/DbBackupsTracker.h \
    src/DbBackupsTrackerController.h \
    src/PromptWidget.h \
    src/DbExportsRegistry.h \
    src/DbExportsRegistryController.h \
    src/DbBackupChangeNumbersComparator.h \
    src/DbMasterController.h \
    src/SystemEventHandler.h \
    src/SystemEvent.h \
    src/RequestLoginNameDialog.h \
    src/RequestDomainSelectionDialog.h \
    src/SystemNotifications/ISystemNotification.h \
    src/SystemNotifications/SystemNotification.h \
    src/BleDev.h \
    src/Settings/DeviceSettings.h \
    src/Settings/DeviceSettingsBLE.h \
    src/Settings/DeviceSettingsMini.h \
    src/Settings/SettingsGuiHelper.h \
    src/Settings/SettingsGuiMini.h \
    src/Settings/SettingsGuiBLE.h \
    src/Settings/ISettingsGui.h \
    src/DeviceDetector.h \
    src/FidoManagement.h

mac {
    HEADERS += src/MacUtils.h \
        src/MacSystemEvents.h \
        src/SystemNotifications/SystemNotificationMac.h \
        src/SystemNotifications/MacNotify.h
    SOURCES += src/SystemNotifications/SystemNotificationMac.cpp
    OBJECTIVE_SOURCES += src/MacUtils.mm \
        src/MacSystemEvents.mm \
        src/SystemNotifications/MacNotify.mm
}

INCLUDEPATH += src\
    src/zxcvbn-c

FORMS    += src/MainWindow.ui \
    src/NotesManagement.ui \
    src/TOTPCredential.ui \
    src/WindowLog.ui \
    src/CredentialsManagement.ui \
    src/FilesManagement.ui \
    src/SSHManagement.ui \
    src/PassGenerationProfilesDialog.ui \
    src/RequestDomainSelectionDialog.ui \
    src/RequestLoginNameDialog.ui \
    src/BleDev.ui \
    src/FidoManagement.ui

RESOURCES += \
    img/images.qrc \
    lang.qrc

win32 {
    RC_FILE = win/windows_res.rc
    HEADERS += src/SystemNotifications/SystemNotificationWindows.h
    SOURCES += src/SystemNotifications/SystemNotificationWindows.cpp

    copydata.commands = $(COPY_FILE) $$shell_path($$PWD\\win\\snoretoast\\*) \"$$shell_path($$OUT_PWD)\"
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}

mac {
    ICON = img/AppIcon.icns
} else {
    ICON = img/AppIcon.svg
}

linux {
    HEADERS += src/SystemNotifications/SystemNotificationUnix.h \
        src/SystemNotifications/SystemNotificationImageUnix.h

    SOURCES += src/SystemNotifications/SystemNotificationUnix.cpp \
        src/SystemNotifications/SystemNotificationImageUnix.cpp
}

unix {
    # INSTALL RULES
    #
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    DEFINES += MC_INSTALL_PREFIX=\\\"$$PREFIX\\\"

    # install the binary
    target.path = $$PREFIX/bin
    INSTALLS += target

    # install the desktop files

    xdgdesktop.path = $$PREFIX/share/applications
    xdgdesktop.extra = "sed 's|PREFIX|$${PREFIX}|' $$PWD/data/moolticute.desktop > $(INSTALL_ROOT)$$xdgdesktop.path/moolticute.desktop"
    INSTALLS += xdgdesktop

    # install metainfo files
    metainfo.path = $$PREFIX/share/metainfo
    metainfo.files = $$PWD/data/moolticute.metainfo.xml
    INSTALLS += metainfo

    # install icons
    iconScalable.path = $$PREFIX/share/icons/hicolor/scalable/apps
    iconScalable.extra = $(INSTALL_FILE) $$PWD/img/AppIcon.svg $(INSTALL_ROOT)$$iconScalable.path/moolticute.svg
    icon32.path = $$PREFIX/share/icons/hicolor/32x32/apps
    icon32.extra = $(INSTALL_FILE) $$PWD/img/AppIcon_32.png $(INSTALL_ROOT)$$icon32.path/moolticute.png
    icon128.path = $$PREFIX/share/icons/hicolor/128x128/apps
    icon128.extra = $(INSTALL_FILE) $$PWD/img/AppIcon_128.png $(INSTALL_ROOT)$$icon128.path/moolticute.png
    INSTALLS += iconScalable icon32 icon128
}

TRANSLATIONS = \
    lang/mc_fr.ts \
    lang/mc_de.ts \
    lang/mc_ru.ts \
    lang/mc_nl.ts \
    lang/mc_ja.ts \
    lang/mc_pt.ts \
    lang/mc_br.ts \
    lang/mc_tr.ts \
    lang/mc_sv.ts \
    lang/mc_hu.ts \
    lang/mc_ko.ts

#Build *.qm translation files automatically

isEmpty(QMAKE_LRELEASE) {
    QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all
