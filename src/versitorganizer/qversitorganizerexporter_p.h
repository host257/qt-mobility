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

#ifndef QVERSITORGANIZEREXPORTER_P_H
#define QVERSITORGANIZEREXPORTER_P_H

#include "qversitorganizerexporter.h"
#include "qorganizerrecurrencerule.h"
#include "qversitorganizerhandler.h"
#include "qversittimezonehandler.h"

QTM_BEGIN_NAMESPACE

class QM_AUTOTEST_EXPORT QVersitOrganizerExporterPrivate
{
public:
    QVersitOrganizerExporterPrivate(const QString& profile = QString());
    ~QVersitOrganizerExporterPrivate();

    bool exportItem(const QOrganizerItem& item,
                    QVersitDocument* document,
                    QVersitOrganizerExporter::Error* error);

    QVersitDocument mResult;
    QMap<int, QVersitOrganizerExporter::Error> mErrors;
    QVersitOrganizerExporterDetailHandler* mDetailHandler;
    QList<QVersitOrganizerHandler*> mPluginDetailHandlers;
    QVersitTimeZoneHandler* mTimeZoneHandler;

private:
    void exportDetail(
            const QOrganizerItem& item,
            const QOrganizerItemDetail& detail,
            QVersitDocument* document);
    void encodeEventTimeRange(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeTodoTimeRange(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeJournalTimeRange(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeTimestamp(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeRecurrence(
            const QOrganizerItem& item,
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeRecurRule(
            const QString& propertyName,
            const QOrganizerRecurrenceRule& rule,
            QList<QVersitProperty>* generatedProperties);
    void appendInts(QString* str, const QSet<int>& ints);
    QString weekString(Qt::DayOfWeek day);
    void encodeRecurDates(
            const QString& propertyName,
            const QOrganizerItem& item,
            const QSet<QDate>& dates,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties);
    void encodePriority(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeInstanceOrigin(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeTodoProgress(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeComment(
            const QOrganizerItemDetail& detail,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    void encodeSimpleProperty(
            const QOrganizerItemDetail& detail,
            const QVersitDocument& document,
            QList<QVersitProperty>* removedProperties,
            QList<QVersitProperty>* generatedProperties,
            QSet<QString>* processedFields);
    QString encodeDateTime(const QDateTime& dateTime);

    bool documentContainsUidAndRecurrenceId(const QVersitDocument& document);
    QVersitProperty takeProperty(
            const QVersitDocument& document,
            const QString& propertyName,
            QList<QVersitProperty>* toBeRemoved);

    // definition name -> <field name, versit property name>:
    QMap<QString, QPair<QString, QString> > mPropertyMappings;
};

QTM_END_NAMESPACE

#endif // QVERSITORGANIZEREXPORTER_P_H
