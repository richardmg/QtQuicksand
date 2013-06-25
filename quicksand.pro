TARGET       = QtQuicksand
TEMPLATE     = app
QT          += core gui declarative svg opengl
contains(QT_CONFIG, release): CONFIG += release

ios: {
    QMAKE_INFO_PLIST = $$PWD/Quicksand-Info.plist
    QTPLUGIN += qtsensors_ios qsvg
    QT     += sensors
    DEFINES += ENABLE_SENSORS
    OTHER_FILES += Quicksand-Info.plist

    icons.files += icon/ios/Icon.png
    icons.files += icon/ios/Icon@2x.png
    icons.files += icon/ios/Icon~ipad.png
    icons.files += icon/ios/Icon~ipad@2x.png
    QMAKE_BUNDLE_DATA += icons
}
android: {
    QT += sensors
    DEFINES += ENABLE_SENSORS
}
INCLUDEPATH += teng_src sandbox_src
SOURCES     += main.cpp\
               teng_src/TSurface_RGBA8888.cpp \
               teng_src/TString.cpp \
               teng_src/TMath.cpp \
               teng_src/TEngine.cpp \
               teng_src/TDrawHelpers.cpp \
               teng_src/TBase.cpp \
               teng_src/T2DMath.cpp \
               sandbox_src/vsand_intro.cpp \
               sandbox_src/vsand_grid.cpp \
               sandbox_src/vsandbox.cpp \
               sandbox_src/table_texture.cpp \
               sandbox_src/sand_texture.cpp \
               sandbox_src/reset_images.cpp \
               sandbox_src/intro_sandpaint.cpp \
               sandbox_src/intro_flare.cpp \
               sandbox_src/intro_digia.cpp \
               sandboxitem.cpp \
               qdeclarativetoucharea.cpp
HEADERS     += \
               teng_src/TSurface_RGBA8888.h \
               teng_src/TString.h \
               teng_src/TMath.h \
               teng_src/TEngine.h \
               teng_src/TDrawHelpers.h \
               teng_src/TBase.h \
               teng_src/T2DMath.h \
               sandbox_src/vsand_intro.h \
               sandbox_src/vsand_grid.h \
               sandbox_src/vsandbox.h \
               sandboxitem.h \
               qdeclarativetoucharea.h
RESOURCES   += quicksand.qrc
OTHER_FILES += main.qml \
               Button.qml \
               Window.qml \
               SandPage.qml \
               FilePage.qml \
               Photo.qml
