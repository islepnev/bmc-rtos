QT -= gui

CONFIG += c++11
CONFIG -= app_bundle

SOURCES += \
    make_sdb.c \
    ../../src/common/sdb_crc16.c \
    ../../src/common/sdb_util.c \
    ../../src/common/str_util.c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../../src/common/sdb_crc16.h \
    ../../src/common/sdb.h \
    ../../src/common/sdb_rom.h \
    ../../src/common/sdb_util.h \
    ../../src/common/str_util.h

LIBS += -lz

INCLUDEPATH += ../../src/common
