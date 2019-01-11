QT              += widgets
TEMPLATE         = lib
CONFIG          += plugin c++11

TARGET           = HTYCMDU
DISTFILES       += cmdu.json

HEADERS += \
    cmduplugin.h \
    cmduwidget.h

SOURCES += \
    cmduplugin.cpp \
    cmduwidget.cpp

RESOURCES += res.qrc