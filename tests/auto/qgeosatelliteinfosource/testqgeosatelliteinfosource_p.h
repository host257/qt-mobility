/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TESTQGEOSATELLITEINFOSOURCE_H
#define TESTQGEOSATELLITEINFOSOURCE_H

#include <qmobilityglobal.h>
#include <QTest>
#include <QObject>


QTM_BEGIN_NAMESPACE
class QGeoSatelliteInfoSource;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class TestQGeoSatelliteInfoSource : public QObject
{
    Q_OBJECT

public:
    TestQGeoSatelliteInfoSource(QObject *parent = 0);

    static TestQGeoSatelliteInfoSource *createDefaultSourceTest();

protected:
    virtual QGeoSatelliteInfoSource *createTestSource() = 0;

    // MUST be called by subclasses if they override respective test slots
    void base_initTestCase();
    void base_init();
    void base_cleanup();
    void base_cleanupTestCase();

public slots:
    void test_slot1();
    void test_slot2();

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void constructor_withParent();
    void constructor_noParent();

    void createDefaultSource();
    void createDefaultSource_noParent();

    void startUpdates();
    void startUpdates_moreThanOnce();
    void stopUpdates();
    void stopUpdates_withoutStart();

    void requestUpdate();
    void requestUpdate_data();

    void requestUpdate_validTimeout();
    void requestUpdate_defaultTimeout();
    void requestUpdate_repeatedCalls();
    void requestUpdate_overlappingCalls();
    void requestUpdate_overlappingCallsWithTimeout();

    void requestUpdateBeforeStartUpdates();
    void requestUpdateAfterStartUpdates();
    
    void removeSlotForRequestTimeout();
    void removeSlotForSatellitesInUseUpdated();
    void removeSlotForSatellitesInViewUpdated();

#ifdef TST_GYPSYMOCK_ENABLED
    // Cases only ran with mock backend
    void updateValues();
    void initGoneBad();
    void badUpdates();
#endif


private:
    QGeoSatelliteInfoSource *m_source;
    bool m_testingDefaultSource;
    bool m_testSlot2Called;
};

#endif // #ifndef TESTQGEOSATELLITEINFOSOURCE_H