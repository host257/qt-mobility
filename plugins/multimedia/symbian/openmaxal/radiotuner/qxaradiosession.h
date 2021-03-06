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

#ifndef QXARADIOSESSION_H
#define QXARADIOSESSION_H

#include <QObject>
#include <QUrl>
#include <qradiotuner.h>
#include "xaradiosessionimplobserver.h"

QT_USE_NAMESPACE

class XARadioSessionImpl;

class QXARadioSession : public QObject, public XARadioSessionImplObserver
{
Q_OBJECT

public:
    QXARadioSession(QObject *parent);
    virtual ~QXARadioSession();

    QRadioTuner::State state() const;
    QRadioTuner::Band band() const;
    void setBand(QRadioTuner::Band band);
    bool isBandSupported(QRadioTuner::Band band) const;
    int frequency() const;
    int frequencyStep(QRadioTuner::Band b) const;
    QPair<int,int> frequencyRange(QRadioTuner::Band b) const;
    void setFrequency(int frequency);
    bool isStereo() const;
    QRadioTuner::StereoMode stereoMode() const;
    void setStereoMode(QRadioTuner::StereoMode mode);
    int signalStrength() const;
    int volume() const;
    int setVolume(int volume);
    bool isMuted() const;
    void setMuted(bool muted);
    bool isSearching() const;
    void searchForward();
    void searchBackward();
    void cancelSearch();
    void start();
    void stop();
    bool isAvailable() const;
    QtMultimediaKit::AvailabilityError availabilityError() const;
    QRadioTuner::Error error() const;
    QString errorString() const;

    /* Callbacks from XARadioSessionImplObserver begin */
    void CBBandChanged(QRadioTuner::Band band);
    void CBStateChanged(QRadioTuner::State state);
    void CBFrequencyChanged(TInt newFrequency);
    void CBStereoStatusChanged(bool isStereo);
    void CBSignalStrengthChanged(int signalStrength);
    void CBVolumeChanged(int volume);
    void CBMutedChanged(bool isMuted);
    void CBSearchingChanged(bool isSearching);
    void CBError(QRadioTuner::Error err);
    /* Callbacks from XARadioSessionImplObserver end */

signals:
    void stateChanged(QRadioTuner::State state);
    void bandChanged(QRadioTuner::Band band);
    void frequencyChanged(int frequency);
    void stereoStatusChanged(bool stereo);
    void searchingChanged(bool stereo);
    void signalStrengthChanged(int signalStrength);
    void volumeChanged(int volume);
    void mutedChanged(bool muted);
    void error(int err, QString str);

private:
    /* Own */
    QRadioTuner::Error iError;
    XARadioSessionImpl* m_impl;
};

#endif /*QXARADIOSESSION_H*/
