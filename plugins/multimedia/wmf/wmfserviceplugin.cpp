/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>
#include <QtCore/QFile>

#include "wmfserviceplugin.h"
#ifdef QMEDIA_MEDIAFOUNDATION_PLAYER
#include "mfplayerservice.h"
#endif
#include <qmediaserviceprovider.h>

QStringList WMFServicePlugin::keys() const
{
    return QStringList()
#ifdef QMEDIA_MEDIAFOUNDATION_PLAYER
            << QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER)
#endif
            ;
}

QMediaService* WMFServicePlugin::create(QString const& key)
{
#ifdef QMEDIA_MEDIAFOUNDATION_PLAYER
    if (key == QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER))
        return new MFPlayerService;
#endif

    //qDebug() << "unsupported key:" << key;
    return 0;
}

void WMFServicePlugin::release(QMediaService *service)
{
    delete service;
}

QMediaServiceProviderHint::Features WMFServicePlugin::supportedFeatures(
        const QByteArray &service) const
{
    if (service == Q_MEDIASERVICE_MEDIAPLAYER)
        return QMediaServiceProviderHint::StreamPlayback;
    else
        return QMediaServiceProviderHint::Features();
}

QList<QByteArray> WMFServicePlugin::devices(const QByteArray &service) const
{
    return QList<QByteArray>();
}

QString WMFServicePlugin::deviceDescription(const QByteArray &service, const QByteArray &device)
{
    return QString();
}

Q_EXPORT_PLUGIN2(qtmedia_wmfengine, WMFServicePlugin);

