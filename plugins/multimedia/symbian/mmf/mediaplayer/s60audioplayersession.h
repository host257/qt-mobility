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

#ifndef S60AUDIOPLAYERSESSION_H
#define S60AUDIOPLAYERSESSION_H

#include "s60mediaplayersession.h"

#ifdef S60_DRM_SUPPORTED
#   include <drmaudiosampleplayer.h>
    typedef CDrmPlayerUtility CAudioPlayer;
    typedef MDrmAudioPlayerCallback MAudioPlayerObserver;
#else
#   include <mdaaudiosampleplayer.h>
    typedef CMdaAudioPlayerUtility CAudioPlayer;
    typedef MMdaAudioPlayerCallback MAudioPlayerObserver;
#endif

#ifdef HAS_AUDIOROUTING
#   include <AudioOutput.h>
#   include <MAudioOutputObserver.h>
#endif

class S60AudioPlayerSession : public S60MediaPlayerSession
                            , public MAudioPlayerObserver
                            , public MAudioLoadingObserver
#ifdef HAS_AUDIOROUTING
                            , public MAudioOutputObserver
#endif
{
    Q_OBJECT
public:
    S60AudioPlayerSession(QObject *parent);
    ~S60AudioPlayerSession();
    
    // S60MediaPlayerSession
    bool isVideoAvailable();
    bool isAudioAvailable();

    // MAudioLoadingObserver
    void MaloLoadingStarted();
    void MaloLoadingComplete();
    
#ifdef HAS_AUDIOROUTING    
    // MAudioOutputObserver
    void DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,
                                   CAudioOutput::TAudioOutputPreference aNewDefault);
#endif

public:
    // S60MediaPlayerAudioEndpointSelector
    QString activeEndpoint() const;
    QString defaultEndpoint() const;
    void setPlaybackRate(qreal rate);

public Q_SLOTS:
    void setActiveEndpoint(const QString &name);

protected:
    // S60MediaPlayerSession
    void doLoadL(const TDesC &path);
    void doLoadUrlL(const TDesC &path);
    void doPlay();
    void doStop();
    void doClose();
    void doPauseL();
    void doSetVolumeL(int volume);
    qint64 doGetPositionL() const;
    void doSetPositionL(qint64 microSeconds);
    void updateMetaDataEntriesL();
    int doGetBufferStatusL() const;
    qint64 doGetDurationL() const;
    void doSetAudioEndpoint(const QString &audioEndpoint);
    bool getIsSeekable() const;

private:
#ifdef S60_DRM_SUPPORTED    
    // MMdaAudioPlayerCallback
    void MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MdapcPlayComplete(TInt aError);
#else
    // MDrmAudioPlayerCallback
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);
    void MapcPlayComplete(TInt aError);
#endif
    
#ifdef HAS_AUDIOROUTING    
    QString qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const;
#endif
    
private:
    CAudioPlayer *m_player;
#ifdef HAS_AUDIOROUTING
    CAudioOutput *m_audioOutput;
#endif
    QString m_audioEndpoint;
};

#endif // S60AUDIOPLAYERSESSION_H
