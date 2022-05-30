QT       += core gui
QT       += serialport
QT       += widgets
QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\OpenCV-4.5.5\opencv\\build\include

LIBS += -lkernel32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -luser32

LIBS += $$PWD\EDSDK_64\Library\EDSDK.lib
dll_files.files += $$PWD\EDSDK_64\Dll\EDSDK.dll
dll_files.files += $$PWD\EDSDK_64\Dll\EdsImage.dll

CONFIG(debug, debug|release) {
  LIBS += C:\OpenCV-4.5.5\opencv\build\x64\vc15\lib\opencv_world455d.lib
  dll_files.files += C:\OpenCV-4.5.5\opencv\build\x64\vc15\bin\opencv_world455d.dll
  OUTDIR = debug
  CONFIG += console
  QMAKE_POST_LINK += $(MAKE) install
}

CONFIG(release, debug|release) {
  LIBS += C:\OpenCV-4.5.5\opencv\build\x64\vc15\lib\opencv_world455.lib
  dll_files.files += C:\OpenCV-4.5.5\opencv\build\x64\vc15\bin\opencv_world455.dll
  OUTDIR = release
  QMAKE_POST_LINK += $(MAKE) install
  QMAKE_POST_LINK += $(MAKE) clean
}

dll_files.path = $$OUT_PWD/$$OUTDIR
INSTALLS += dll_files

SOURCES += \
    Autostitch.cpp \
    CameraStageController.cpp \
    CanonCamera.cpp \
    GCodeSender.cpp \
    Logger.cpp \
    MotorController.cpp \
    Scripter.cpp \
    SerialClass.cpp \
    Stitcher.cpp \
    StitcherWorker.cpp \
    dialogs/autohelpdialog.cpp \
    dialogs/camerasettingsdialog.cpp \
    dialogs/helpdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    dialogs/stitcherhelpdialog.cpp

HEADERS += \
    Autostitch.h \
    CameraStageController.h \
    CanonCamera.h \
    EDSDK/Header/EDSDK.h \
    EDSDK/Header/EDSDKErrors.h \
    EDSDK/Header/EDSDKTypes.h \
    GCodeSender.h \
    Logger.h \
    MotorController.h \
    Scripter.h \
    SerialClass.h \
    Stitcher.h \
    StitcherWorker.h \
    dialogs/autohelpdialog.h \
    dialogs/camerasettingsdialog.h \
    dialogs/helpdialog.h \
    mainwindow.h \
    dialogs/stitcherhelpdialog.h

FORMS += \
    ui_files/autohelpdialog.ui \
    ui_files/stitcherhelpdialog.ui \
    ui_files/camerasettingsdialog.ui \
    ui_files/mainwindow.ui \
    ui_files/helpdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource/stitching.qrc
