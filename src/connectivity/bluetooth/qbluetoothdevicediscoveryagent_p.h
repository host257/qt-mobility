/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBLUETOOTHDEVICEDISCOVERYAGENT_P_H
#define QBLUETOOTHDEVICEDISCOVERYAGENT_P_H

#include "qbluetoothdevicediscoveryagent.h"

#include <QVariantMap>

#ifndef QT_NO_DBUS
class OrgBluezManagerInterface;
class OrgBluezAdapterInterface;
class QDBusVariant;
#endif

#ifdef Q_OS_SYMBIAN
#include <es_sock.h>
#include <bt_sock.h>
#endif

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothDeviceDiscoveryAgentPrivate
#ifdef Q_OS_SYMBIAN
: public CActive
#endif
{
    Q_DECLARE_PUBLIC(QBluetoothDeviceDiscoveryAgent)
public:
    QBluetoothDeviceDiscoveryAgentPrivate();
    ~QBluetoothDeviceDiscoveryAgentPrivate();

    void start();
    void stop();
    bool isActive() const;

#ifdef Q_OS_SYMBIAN
    void RunL();
    void DoCancel();

    RHostResolver hostResolver;
    RSocketServ socketServer;
    TNameEntry entry;
#endif

#ifndef QT_NO_DBUS
    void _q_deviceFound(const QString &address, const QVariantMap &dict);
    void _q_propertyChanged(const QString &name, const QDBusVariant &value);
#endif
    
    QList<QBluetoothDeviceInfo> discoveredDevices;
    QBluetoothDeviceDiscoveryAgent::InquiryType inquiryType;
    QBluetoothDeviceDiscoveryAgent *q;   

#if !defined(QT_NO_DBUS)
    OrgBluezManagerInterface *manager;
    OrgBluezAdapterInterface *adapter;
#endif

    QBluetoothDeviceDiscoveryAgent *q_ptr;
    
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif