QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

SOURCES += \
        main.c \
        lldp_message.c \
        pcap_file.c

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    lldp_message.h \
    lldp_tlv.h \
    pcap_file.h

DEFINES += NO_LWIP
