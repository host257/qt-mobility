
HEADERS += \
    bluetooth/qbluetoothaddress.h\
    bluetooth/qbluetoothuuid.h\
    bluetooth/qbluetoothdeviceinfo.h\
    bluetooth/qbluetoothserviceinfo.h\
    bluetooth/qbluetoothdevicediscoveryagent.h\
    bluetooth/qbluetoothservicediscoveryagent.h\
    bluetooth/qbluetoothsocket.h\
    bluetooth/qrfcommserver.h \
    bluetooth/ql2capserver.h \
    bluetooth/qbluetooth.h \
    bluetooth/qbluetoothlocaldevice.h \
    bluetooth/ql2capsocket.h \
    bluetooth/qrfcommsocket.h \
    bluetooth/qbluetoothtransfermanager.h \
    bluetooth/qbluetoothtransferrequest.h \
    bluetooth/qbluetoothtransferreply.h

PRIVATE_HEADERS += \
    bluetooth/qbluetoothuuid_p.h\
    bluetooth/qbluetoothdeviceinfo_p.h\
    bluetooth/qbluetoothserviceinfo_p.h\
    bluetooth/qbluetoothdevicediscoveryagent_p.h\
    bluetooth/qbluetoothservicediscoveryagent_p.h\
    bluetooth/qbluetoothsocket_p.h\
    bluetooth/qrfcommserver_p.h \
    bluetooth/ql2capserver_p.h \
    bluetooth/qbluetoothtransferrequest_p.h

SOURCES += \
    bluetooth/qbluetoothaddress.cpp\
    bluetooth/qbluetoothuuid.cpp\
    bluetooth/qbluetoothdeviceinfo.cpp\
    bluetooth/qbluetoothserviceinfo.cpp\
    bluetooth/qbluetoothdevicediscoveryagent.cpp\
    bluetooth/qbluetoothservicediscoveryagent.cpp\
    bluetooth/qbluetoothsocket.cpp\
    bluetooth/qrfcommserver.cpp \
    bluetooth/ql2capserver.cpp \
    bluetooth/qbluetoothlocaldevice.cpp \
    bluetooth/qbluetooth.cpp \
    bluetooth/ql2capsocket.cpp \
    bluetooth/qrfcommsocket.cpp \
    bluetooth/qbluetoothtransfermanager.cpp \
    bluetooth/qbluetoothtransferrequest.cpp \
    bluetooth/qbluetoothtransferreply.cpp

symbian {

    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

    PRIVATE_HEADERS += \
        bluetooth/utils_symbian_p.h

    SOURCES += \
        bluetooth/qbluetoothserviceinfo_symbian.cpp\     
        bluetooth/qbluetoothdevicediscoveryagent_symbian.cpp\
        bluetooth/qbluetoothservicediscoveryagent_symbian.cpp\
        bluetooth/qbluetoothsocket_symbian.cpp\
        bluetooth/qrfcommserver_symbian.cpp \
        bluetooth/qbluetoothlocaldevice_symbian.cpp \
        bluetooth/qbluetoothtransfermanager_symbian.cpp \
        bluetooth/ql2capserver_symbian.cpp
        
        
    LIBS *= -lesock -lbluetooth -lsdpagent -lsdpdatabase -lestlib
} else:contains(QT_CONFIG, dbus) {
    QT *= dbus

    include(bluez/bluez.pri)

    PRIVATE_HEADERS += \
        bluetooth/qbluetoothtransferreply_bluez_p.h

    SOURCES += \
        bluetooth/qbluetoothserviceinfo_bluez.cpp \                
        bluetooth/qbluetoothdevicediscoveryagent_bluez.cpp\
        bluetooth/qbluetoothservicediscoveryagent_bluez.cpp \
        bluetooth/qbluetoothsocket_bluez.cpp \
        bluetooth/qrfcommserver_bluez.cpp \
        bluetooth/qbluetoothlocaldevice_bluez.cpp \
        bluetooth/qbluetoothtransferreply_bluez.cpp \
        bluetooth/qbluetoothtransfermanager_bluez.cpp \
        bluetooth/ql2capserver_bluez.cpp
}

INCLUDEPATH += $$PWD
INCLUDEPATH += ..
INCLUDEPATH += $${QMAKE_INCDIR_QT}/QtNetwork

OTHER_FILES +=