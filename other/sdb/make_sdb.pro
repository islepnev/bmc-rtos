QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += \
    make_sdb.c \
    ../../src/common/sdb_util.c \
    ../../src/common/str_util.c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    sdb.h

LIBS += -lz

INCLUDEPATH += ../../src/common
