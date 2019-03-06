#-------------------------------------------------
#
# Project created by QtCreator 2018-03-14T23:40:39
#
#-------------------------------------------------

QT       -= core gui
TEMPLATE = lib
CONFIG +=warn_on plugin release
CONFIG-=thread exceptions qt rtti
CONFIG += c++11

#DEFINES += testVR_LIBRARY

INCLUDEPATH += C:\SDK\CHeaders\XPLM\
INCLUDEPATH += C:\SDK\CHeaders\Wrappers\
INCLUDEPATH += C:\SDK\CHeaders\Widgets\
INCLUDEPATH +=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17134.0\um\arm64

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES +=XPLM301
DEFINES +=XPLM300
DEFINES +=XPLM210
DEFINES +=XPLM200
DEFINES +=APL=0 IBM=1 LIN=0
LIBS +=-LC:\SDK\Libraries\Win
LIBS +=-lXPLM_64 -lXPWidgets_64
LIBS +=-lOpengl32 # lodbc32 lodbccp32
LIBS +=User32.lib


TARGET = VR_Tools.xpl

SOURCES += \
    stringops.cpp \
    textreader.cpp \
    textline.cpp \
    rectangles.cpp \
    layout.cpp \
    point.cpp \
    showdir.cpp \
    button_vr.cpp \
    scrollbars.cpp \
    list_box_with_scrb.cpp \
    dirnamereader.cpp \
    filenamepicker.cpp \
    VR_Tools.cpp \
    inisettings.cpp \
    key.cpp \
    keyboard.cpp \
    layoutwithedit.cpp \
    textedit.cpp \
    cursor.cpp \
    drefwindow.cpp \
    vrcommandfilter.cpp \
    localclipboard.cpp \
    filepointer.cpp \
    opcenter.cpp \
    drawlogic.cpp \
    temporarywindow.cpp \
    modalbutton.cpp \
    managemodalwindow.cpp \
    alert3buttons.cpp \
    linedialog.cpp \
    vrviews.cpp \
    vrcreader.cpp

HEADERS += \
    stringops.h \
    textreader.h \
    textline.h \
    rectangles.h \
    layout.h \
    point.h \
    showdir.h \
    button_vr.h \
    scrollbars.h \
    list_box_with_scrb.h \
    dirnamereader.h \
    filenamepicker.h \
    VR_Tools.h \
    VR_Tools_global.h \
    inisettings.h \
    key.h \
    keyboard.h \
    layoutwithedit.h \
    textedit.h \
    cursor.h \
    drefwindow.h \
    vrcommandfilter.h \
    localclipboard.h \
    filepointer.h \
    opcenter.h \
    drawlogic.h \
    temporarywindow.h \
    modalbutton.h \
    managemodalwindow.h \
    alert3buttons.h \
    linedialog.h \
    vrviews.h \
    vrcreader.h

target.path= \plugsXP
unix {
    target.path = /usr/lib
    INSTALLS += target
}
