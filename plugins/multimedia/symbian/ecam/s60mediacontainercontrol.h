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

#ifndef S60MEDIACONTAINERCONTROL_H
#define S60MEDIACONTAINERCONTROL_H

#include <QtCore/qstringlist.h>
#include <QtCore/qmap.h>
#include <qmediacontainercontrol.h>

QT_USE_NAMESPACE

class S60VideoCaptureSession;

/*
 * Control for setting container (file format) for video recorded using
 * QMediaRecorder.
 */
class S60MediaContainerControl : public QMediaContainerControl
{
    Q_OBJECT

public: // Contructors & Destructor

    S60MediaContainerControl(QObject *parent = 0);
    S60MediaContainerControl(S60VideoCaptureSession *session, QObject *parent = 0);
    virtual ~S60MediaContainerControl();

public: // QMediaContainerControl

    QStringList supportedContainers() const;
    QString containerMimeType() const;
    void setContainerMimeType(const QString &containerMimeType);

    QString containerDescription(const QString &containerMimeType) const;

private: // Data

    S60VideoCaptureSession  *m_session;
    QStringList             m_supportedContainers;
    QMap<QString, QString>  m_containerDescriptions;
};

#endif // S60MEDIACONTAINERCONTROL_H
