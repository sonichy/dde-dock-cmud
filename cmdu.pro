QT              += widgets
TEMPLATE         = lib
CONFIG          += c++11 plugin

TARGET           = HTYCMDU
DISTFILES       += cmdu.json

HEADERS += \
    cmduplugin.h \
    cmduwidget.h

SOURCES += \
    cmduplugin.cpp \
    cmduwidget.cpp

RESOURCES += res.qrc