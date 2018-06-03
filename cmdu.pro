
#include(../dde-dock/interfaces/interfaces.pri)

QT              += widgets svg
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       +=

#TARGET          = $$qtLibraryTarget(datetime)
TARGET           = CMDU
#DESTDIR          = $$_PRO_FILE_PWD_/../
DISTFILES       += \
    cmdu.json

HEADERS += \
    cmduplugin.h \
    cmduwidget.h

SOURCES += \
    cmduplugin.cpp \
    cmduwidget.cpp

target.path = $${PREFIX}/lib/dde-dock/plugins/
INSTALLS += target

RESOURCES += \
    resources.qrc
