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

#include "mainwindow.h"

#include <QCoreApplication>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QAction>
#include <QVBoxLayout>

MainWindow::MainWindow() :
    serviceProvider(0),
    markerManager(0),
    positionSource(0),
    tracking(true),
    firstUpdate(true)
{
    mapsWidget = new MapsWidget;
    setCentralWidget(mapsWidget);

    QMenuBar *mbar = new QMenuBar(this);
    mbar->addAction("My Location", this, SLOT(goToMyLocation()));

    QMenu *searchMenu = new QMenu("Search");
    mbar->addMenu(searchMenu);

    searchMenu->addAction("For address or name", this, SLOT(showSearchDialog()));

    setMenuBar(mbar);

    initialize();
}

MainWindow::~MainWindow()
{
    delete mapsWidget;
    if (serviceProvider)
        delete serviceProvider;
    if (markerManager)
        delete markerManager;
}

void MainWindow::goToMyLocation()
{
    mapsWidget->animatedPanTo(markerManager->myLocation());
    tracking = true;
}

void MainWindow::initialize()
{
    if (serviceProvider)
        delete serviceProvider;

    QList<QString> providers = QGeoServiceProvider::availableServiceProviders();
    if (providers.size() < 1) {
        QMessageBox::information(this, tr("Maps Navigator"),
                                 tr("No service providers are available"));
        QCoreApplication::quit();
        return;
    }

    serviceProvider = new QGeoServiceProvider(providers[0]);
    if (serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("Maps Navigator"),
                                 tr("Error loading geoservice plugin: %1").arg(providers[0]));
        QCoreApplication::quit();
        return;
    }

    mapsWidget->initialize(serviceProvider->mappingManager());

    if (markerManager)
        delete markerManager;
    markerManager = new MarkerManager(serviceProvider->searchManager());
    mapsWidget->setMarkerManager(markerManager);

    connect(markerManager, SIGNAL(searchError(QGeoSearchReply::Error,QString)),
            this, SLOT(showErrorMessage(QGeoSearchReply::Error,QString)));
    connect(mapsWidget, SIGNAL(markerClicked(Marker*)),
            this, SLOT(on_markerClicked(Marker*)));
    connect(mapsWidget, SIGNAL(mapPanned()),
            this, SLOT(disableTracking()));

    if (positionSource)
        delete positionSource;

    positionSource = QGeoPositionInfoSource::createDefaultSource(this);

    if (!positionSource) {
        mapsWidget->statusBar()->showText("Could not open GPS", 5000);
        mapsWidget->setMyLocation(QGeoCoordinate(-27.5796, 153.1));
    } else {
        positionSource->setUpdateInterval(1000);
        connect(positionSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
                this, SLOT(updateMyPosition(QGeoPositionInfo)));
        positionSource->startUpdates();
        mapsWidget->statusBar()->showText("Opening GPS...");
    }
}

void MainWindow::disableTracking()
{
    tracking = false;
}

void MainWindow::updateMyPosition(QGeoPositionInfo info)
{
    if (mapsWidget) {
        mapsWidget->setMyLocation(info.coordinate(), false);
        if (tracking)
            mapsWidget->animatedPanTo(info.coordinate());
    }
    if (firstUpdate) {
        mapsWidget->statusBar()->showText("Receiving from GPS");
        firstUpdate = false;
    }
}

void MainWindow::showSearchDialog()
{
    SearchDialog sd;
    if (sd.exec() == QDialog::Accepted) {
        if (markerManager) {
            markerManager->removeSearchMarkers();
            markerManager->search(sd.searchTerms(), sd.radius());
        }
    }
}

void MainWindow::showErrorMessage(QGeoSearchReply::Error err, QString msg)
{
    QMessageBox::critical(this, tr("Error"), msg);
    mapsWidget->statusBar()->hide();
}

void MainWindow::on_markerClicked(Marker *marker)
{
    MarkerDialog md(marker);
    if (md.exec() == QDialog::Accepted) {
        md.updateMarker();
    }
}