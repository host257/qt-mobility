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

#include <QObject>
#include <qmobilityglobal.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif



#define QTRY_COMPARE_SIGNAL_COUNTS() \
    QTRY_COMPARE(spyAdded.count(), itemsAddedSignals); \
    QTRY_COMPARE(spyChanged.count(), itemsChangedSignals); \
    QTRY_COMPARE(spyRemoved.count(), itemsRemovedSignals); \
    QTRY_COMPARE(spyDataChanged.count(), dataChangedSignals); \
    if (spyAdded.count()) \
        { QCOMPARE(spyAdded.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsAdded); } \
    if (spyChanged.count()) \
        { QCOMPARE(spyChanged.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsChanged); } \
    if (spyRemoved.count()) \
        { QCOMPARE(spyRemoved.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsRemoved); }

#define QTRY_COMPARE_SIGNAL_COUNTS2() \
    QTRY_COMPARE(spyAdded2.count(), itemsAddedSignals); \
    QTRY_COMPARE(spyChanged2.count(), itemsChangedSignals); \
    QTRY_COMPARE(spyRemoved2.count(), itemsRemovedSignals); \
    QTRY_COMPARE(spyDataChanged2.count(), dataChangedSignals); \
    if (spyAdded2.count()) \
        { QCOMPARE(spyAdded2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsAdded); } \
    if (spyChanged2.count()) \
        { QCOMPARE(spyChanged2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsChanged); } \
    if (spyRemoved2.count()) \
        { QCOMPARE(spyRemoved2.last().at(0).value<QList<QOrganizerItemLocalId> >().count(), itemsRemoved); }


const QString m_managerNameSymbian("symbian");

/*!
 * A test helper class that acts as a placeholder for QOrganizerItemDetail data.
 */
class QTstDetailField
{
public:
    QTstDetailField(QString definitionName, QString fieldName, QVariant value)
    {
        m_definitionName = definitionName;
        m_fieldName = fieldName;
        m_value = value;
    }
    QString m_definitionName;
    QString m_fieldName;
    QVariant m_value;
};

typedef QList<QTstDetailField> QTstDetailFieldList;
Q_DECLARE_METATYPE(QTstDetailFieldList)

Q_DECLARE_METATYPE(QList<QOrganizerItemLocalId>)

/*!
 * For testing symbian backend via QOrganizerItemManager API. The target is
 * to implement test cases in a platform independent manner so that this test
 * module could be used also with other backends and potentially used as a
 * QtMobility auto test with as little porting work as possible.
 */
class tst_SymbianOm : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases

    void addSimpleItem_data(){ addManagers(); };
    void addSimpleItem();
    void fetchSimpleItem_data(){ addManagers(); };
    void fetchSimpleItem();
    void removeSimpleItem_data(){ addManagers(); };
    void removeSimpleItem();
    void fetchItems_data(){ addManagers(); };
    void fetchItems();
    void fetchItemIds_data(){ addManagers(); };
    void fetchItemIds();
    void uniqueIds_data(){ addManagers(); };
    void uniqueIds();
    void timeStamp_data(){ addManagers(); };
    void timeStamp();
    void addItem_data();
    void addItem();
    void signalEmission_data(){ addManagers(); };
    void signalEmission();
    void invalidDetail();
    void invalidDetail_data(){ addManagers(); };

private:
    // TODO: enable the following test cases by moving them to "private slots"
    // when the required functionality has been implemented in symbian backend

    // Negative test cases (testing for example with illegal parameters)
    void addNegative_data(){ addManagers(); };
    void addNegative();
    void fetchNegative_data(){ addManagers(); };
    void fetchNegative();
    void removeNegative_data(){ addManagers(); };
    void removeNegative();

private: // util functions
    void addManagers();
    void addItem_dataBasic(QString managerName, QString itemType);
    void addItem_dataEventTimeRange(QString managerName, QString itemType);
    void addItem_dataTodoTimeRange(QString managerName, QString itemType);
    void addItem_dataReminders(QString managerName, QString itemType);
    void addItem_todo(QString managerName);
    QString testCaseName(QString testCase, QString managerName, QString itemType);
    bool parseDetails(QTstDetailFieldList detailsString, QList<QOrganizerItemDetail> &details);
    bool verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected);
    bool isKnownIssue(QOrganizerItemDetail actual, QOrganizerItemDetail expected);
    bool compareItemLists(QList<QOrganizerItem> actual, QList<QOrganizerItem> expected, QStringList ignoredDetails = QStringList());
    bool compareItems(QOrganizerItem itemA, QOrganizerItem itemB, QStringList ignoredDetails = QStringList());    

private:
    QOrganizerItemManager *m_om;
};


void tst_SymbianOm::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerItemManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
}

void tst_SymbianOm::cleanup()
{
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
    delete m_om;
    m_om = 0;
}

void tst_SymbianOm::addSimpleItem()
{
    // Create item and set it's details
    QOrganizerItem item;
    // This is the simplest possible item, not even a display label or time
    // range. The item type used is TypeTodo because adding event without
    // time range will crash (panic "Agenda model" 1) on symbian.
    item.setType(QOrganizerItemType::TypeTodo);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Save with list parameter
    QList<QOrganizerItem> items;
    items.append(item);
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    foreach (QOrganizerItem listitem, items) {
        QVERIFY(listitem.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));
    }

    // Save with list parameter and error map parameter
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), &errorMap));
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QVERIFY(errorMap.count() == 0);
    foreach (QOrganizerItem listitem2, items) {
        QVERIFY(listitem2.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));
    }
}

void tst_SymbianOm::fetchSimpleItem()
{
    // Create item
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeTodo);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Fetch
    QOrganizerItem fetchedItem = m_om->item(item.localId());
    QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
    QCOMPARE(fetchedItem.id(), item.id());
    QVERIFY(fetchedItem.type() == QOrganizerItemType::TypeTodo);
}

void tst_SymbianOm::removeSimpleItem()
{
    // Create item
    QOrganizerItem item;
    item.setType(QOrganizerItemType::TypeTodo);

    // Save
    QVERIFY(m_om->saveItem(&item));
    QVERIFY(item.id().localId() != 0);
    QVERIFY(item.id().managerUri().contains(m_om->managerName()));

    // Remove
    QVERIFY(m_om->removeItem(item.localId()));
    
    // Remove again. Should fail.
    QVERIFY(!m_om->removeItem(item.localId()));

    // Remove list
    QOrganizerItem item2;
    item2.setType(QOrganizerItemType::TypeTodo);
    QOrganizerItem item3;
    item3.setType(QOrganizerItemType::TypeTodo);
    QVERIFY(m_om->saveItem(&item2));
    QVERIFY(m_om->saveItem(&item3));
    QList<QOrganizerItemLocalId> itemIds;
    itemIds.append(item2.localId());
    itemIds.append(item3.localId());
    QMap<int, QOrganizerItemManager::Error> errorMap;
    QVERIFY(m_om->removeItems(itemIds, &errorMap));

    // Remove with itemIds
    QOrganizerItem item4;
    item4.setType(QOrganizerItemType::TypeTodo);
    QOrganizerItem item5;
    item5.setType(QOrganizerItemType::TypeTodo);
    QVERIFY(m_om->saveItem(&item4));
    QVERIFY(m_om->saveItem(&item5));
    QVERIFY(m_om->removeItems(m_om->itemIds(), 0));
}

void tst_SymbianOm::fetchItems()
{
    // Add some items
    QList<QOrganizerItem> expectedItems;
    for (int i=0; i<10; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        expectedItems << item;
    }

    // Fetch ids
    QList<QOrganizerItem> actualItems = m_om->items();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    
    // Verify
    QVERIFY(compareItemLists(actualItems, expectedItems));
}

void tst_SymbianOm::fetchItemIds()
{
    // Add some items
    QList<QOrganizerItemLocalId> expectedIds;
    for (int i=0; i<10; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        expectedIds << item.localId();
    }

    // Fetch ids
    QList<QOrganizerItemLocalId> actualIds = m_om->itemIds();
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    
    // Verify
    QVERIFY(actualIds.count() == expectedIds.count());
    foreach (QOrganizerItemLocalId id, actualIds) {
        QVERIFY(id != QOrganizerItemLocalId());
        QVERIFY(expectedIds.contains(id));
    }
}

void tst_SymbianOm::uniqueIds()
{
    const int count = 10;
    QList<QOrganizerItem> items;
    QList<QOrganizerItemLocalId> lids;
    QList<QString> guids;

    // Test that new items have unique ids
    for (int i=0; i<count; i++) {
        QOrganizerItem item;
        item.setType(QOrganizerItemType::TypeTodo);
        QVERIFY(m_om->saveItem(&item));
        QVERIFY(!lids.contains(item.localId()));
        QVERIFY(!guids.contains(item.guid()));
        lids << item.localId();
        guids << item.guid();
        items << item;
    }
    
    // Modify and check that local id and global uid persists
    for (int i=0; i<count; i++) {
        QOrganizerItem &item = items[i];
        item.setDisplayLabel("foobar");
        QVERIFY(m_om->saveItem(&item));
        QVERIFY(lids[i] == item.localId());
        QVERIFY(guids[i] == item.guid());
    }
    
    // Load and check that ids are the still the same
    for (int i=0; i<count; i++) {
        QOrganizerItem item = m_om->item(lids[i]);
        QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
        QVERIFY(lids[i] == item.localId());
        QVERIFY(guids[i] == item.guid());
    }
    
    // Save a new todo item with own localid. Should fail.
    QOrganizerTodo todo;
    QOrganizerItemId id;
    id.setLocalId(12345);
    todo.setId(id);
    QVERIFY(!m_om->saveItem(&todo));
    
    // Save a new todo item with own guid. Should pass.
    todo = QOrganizerTodo();
    todo.setGuid("1234567890");
    QVERIFY(m_om->saveItem(&todo));
    QVERIFY(todo.guid() == "1234567890");
    
    // Save an existing todo item with guid only. Should pass.
    QOrganizerItemLocalId localId = todo.localId();
    todo.setDescription("foobar");
    todo.setId(QOrganizerItemId());
    QVERIFY(m_om->saveItem(&todo));
    QVERIFY(todo.localId() == localId); // local id should remain the same
    
    // Change manager uri and save again. Should fail.
    id = todo.id();
    id.setManagerUri("foobar");
    todo.setId(id);
    QVERIFY(!m_om->saveItem(&todo));    
        
    // Save a new todo item with own guid & localid. Should fail.
    todo = QOrganizerTodo();
    id = QOrganizerItemId();
    id.setLocalId(12345);
    todo.setId(id);
    todo.setGuid("11111");
    QVERIFY(!m_om->saveItem(&todo));
}

void tst_SymbianOm::timeStamp()
{
    // TODO: support for created time? is it possible with symbian?
    // TODO: check detail definitions
    
    QDateTime startTime = QDateTime::currentDateTime();
    //qDebug() << "starttime:" << startTime;
    
    // Save item
    QOrganizerTodo item1;
    item1.setDisplayLabel("do stuff");
    QVERIFY(m_om->saveItem(&item1));
    
    // Verify timestamp
    QOrganizerItemTimestamp timeStamp1 = item1.detail<QOrganizerItemTimestamp>();
    //qDebug() << "timestamp1" << timeStamp1;
    QVERIFY(!timeStamp1.isEmpty());
    QVERIFY(timeStamp1.created().isValid());
    QVERIFY(timeStamp1.lastModified().isValid());
    QVERIFY(timeStamp1.created() >= startTime);
    QVERIFY(timeStamp1.lastModified() >= startTime);    
        
    // Wait a while
    QTest::qSleep(1000);

    // Save again
    item1.setDisplayLabel("do more stuff");
    QVERIFY(m_om->saveItem(&item1));
    
    // Verify timestamp
    QOrganizerItemTimestamp timeStamp2 = item1.detail<QOrganizerItemTimestamp>();
    //qDebug() << "timestamp2" << timeStamp2;
    QVERIFY(!timeStamp2.isEmpty());
    QVERIFY(timeStamp2.created().isValid());
    QVERIFY(timeStamp2.created() == timeStamp1.created());
    QVERIFY(timeStamp2.lastModified().isValid());
    QVERIFY(timeStamp2.lastModified() > timeStamp1.lastModified());
        
    // Load the same item again
    QOrganizerTodo item2 = m_om->item(item1.localId());
    QVERIFY(m_om->error() == QOrganizerItemManager::NoError);
    QOrganizerItemTimestamp timeStamp3 = item2.detail<QOrganizerItemTimestamp>();
    //qDebug() << "timestamp3" << timeStamp3;
    QVERIFY(timeStamp3.lastModified() == timeStamp2.lastModified());
}

void tst_SymbianOm::addNegative()
{
    // Save with null pointer as item
    QVERIFY(!m_om->saveItem(0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QVERIFY(!m_om->saveItems(0, 0, 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    QList<QOrganizerItem> items;
    QVERIFY(!m_om->saveItems(&items, QOrganizerCollectionLocalId(), 0));
    QCOMPARE(m_om->error(), QOrganizerItemManager::BadArgumentError);

    // TODO: try to save an event with non-existing (non-zero) id and check that it fails
}

void tst_SymbianOm::fetchNegative()
{
    // TODO: negative cases: fetch with non-existing id, ...
}

void tst_SymbianOm::removeNegative()
{
    // TODO: negative cases: remove with empty list, non-existing item, multiple non-existing items...
}

void tst_SymbianOm::addItem_data()
{
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<int>("expectedError");
    QTest::addColumn<QTstDetailFieldList>("detailsList");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    managerNames.removeAll("memory");

    foreach (QString managerName, managerNames) {
        // Item type event
        addItem_dataBasic(managerName, QOrganizerItemType::TypeEvent);
        addItem_dataEventTimeRange(managerName, QOrganizerItemType::TypeEvent);
        addItem_dataReminders(managerName, QOrganizerItemType::TypeEvent);

        addItem_dataBasic(managerName, QOrganizerItemType::TypeTodo);
        addItem_dataTodoTimeRange(managerName, QOrganizerItemType::TypeTodo);
        addItem_dataReminders(managerName, QOrganizerItemType::TypeTodo);

        // Journal not implementable on symbian backend
        //addItem_dataBasic(managerName, QOrganizerItemType::TypeJournal);

        // Note not supported in all platforms
#ifdef AGENDA_EXT_SUPPORT
        addItem_dataBasic(managerName, QOrganizerItemType::TypeNote);
#endif
        //addItem_todo(managerName);
    }
}

void tst_SymbianOm::addItem()
{
    // Fetch parameters
    QFETCH(int, expectedError);
    QFETCH(QTstDetailFieldList, detailsList);

    QOrganizerItemManager::Error expectedErrorCode = (QOrganizerItemManager::Error) expectedError; 
    QList<QOrganizerItemDetail> details;
    QVERIFY(parseDetails(detailsList, details));

    // Initialize: Create test item and set it's details
    QOrganizerItem item;
    foreach(QOrganizerItemDetail detail, details) {
        QVERIFY(item.saveDetail(&detail));
        /* TODO: enable the following and implement detail definitions to symbian backend 
        QOrganizerItemDetailDefinition def = m_om->detailDefinition(detail.definitionName(), item.type());
        if (m_om->error() != QOrganizerItemManager::NoError) {
            // this detail is not supported, skip the test case
            QString skipMsg = QString("Detail ") + detail.definitionName() + QString(" not supported for item type " + item.type());
            QSKIP(skipMsg.toAscii(), SkipSingle);
        }
        */
    }

    // Add, fetch and verify result
    if (expectedErrorCode == QOrganizerItemManager::NoError) {
        // Save
        QVERIFY(m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
        QVERIFY(item.id().localId() != 0);
        QVERIFY(item.id().managerUri().contains(m_om->managerName()));

        // Fetch item to verify everything was saved successfully
        QOrganizerItem savedItem = m_om->item(item.localId());
        QCOMPARE(m_om->error(), QOrganizerItemManager::NoError);
        QVERIFY(verifyDetails(savedItem.details(), details));
    } else {
        // An error is expected -> Check that save fails and the error code
        // is the correct one
        QVERIFY(!m_om->saveItem(&item));
        QCOMPARE(m_om->error(), expectedErrorCode);
    }
}

void tst_SymbianOm::signalEmission()
{
    QFETCH(QString, managerName);
    
    // Create a second manager
    QOrganizerItemManager om2(managerName);
    
    // Connect signal spies
    qRegisterMetaType<QOrganizerItemLocalId>("QOrganizerItemLocalId");
    qRegisterMetaType<QList<QOrganizerItemLocalId> >("QList<QOrganizerItemLocalId>");
    QSignalSpy spyAdded(m_om, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyChanged(m_om, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyRemoved(m_om, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyDataChanged(m_om, SIGNAL(dataChanged()));
    QSignalSpy spyAdded2(&om2, SIGNAL(itemsAdded(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyChanged2(&om2, SIGNAL(itemsChanged(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyRemoved2(&om2, SIGNAL(itemsRemoved(QList<QOrganizerItemLocalId>)));
    QSignalSpy spyDataChanged2(&om2, SIGNAL(dataChanged()));
    int itemsAddedSignals = 0;
    int itemsChangedSignals = 0;
    int itemsRemovedSignals = 0;
    int dataChangedSignals = 0;
    int itemsAdded = 0;
    int itemsChanged = 0;
    int itemsRemoved = 0;
    
    // Save
    QOrganizerTodo todo;
    QVERIFY(m_om->saveItem(&todo));
    itemsAddedSignals++;
    itemsAdded = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Change
    todo.setDescription("foobar");
    QVERIFY(m_om->saveItem(&todo));
    itemsChangedSignals++;
    itemsChanged = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Change but with guid only (local id not defined)
    todo.setDescription("barfoo");
    todo.setId(QOrganizerItemId());
    QVERIFY(m_om->saveItem(&todo));
    itemsChangedSignals++;
    itemsChanged = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();    
    
    // Remove
    QVERIFY(m_om->removeItem(todo.localId()));
    itemsRemovedSignals++;
    itemsRemoved = 1;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Save - batch
    todo.setId(QOrganizerItemId());
    todo.setGuid(QString());
    QOrganizerTodo todo2;
    QList<QOrganizerItem> items;
    items << todo;
    items << todo2;
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), 0));
    itemsAddedSignals++;
    itemsAdded = 2;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Change - batch
    items[0].setDescription("foobar1");
    items[1].setDescription("foobar2");
    QVERIFY(m_om->saveItems(&items, QOrganizerCollectionLocalId(), 0));
    itemsChangedSignals++;
    itemsChanged = 2;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
    
    // Remove - batch
    QList<QOrganizerItemLocalId> itemIds;
    itemIds << items[0].localId();
    itemIds << items[1].localId();
    QVERIFY(m_om->removeItems(itemIds, 0));
    itemsRemovedSignals++;
    itemsRemoved = 2;
    QTRY_COMPARE_SIGNAL_COUNTS();
    QTRY_COMPARE_SIGNAL_COUNTS2();
}

void tst_SymbianOm::invalidDetail()
{
    // NOTE: There is an auto test about this (tst_QOrganizerItemManager::itemValidation)
    // but its not working currently on symbian backend. This test can be removed
    // when it's compatible with symbian backend.
    QOrganizerTodo todo;
    QOrganizerItemDetail invalidDetail("invalid");
    invalidDetail.setValue("foo", "bar");
    QVERIFY(todo.saveDetail(&invalidDetail));
    QVERIFY(!m_om->saveItem(&todo));
    QVERIFY(m_om->error() == QOrganizerItemManager::InvalidDetailError);
}

/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_SymbianOm::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend
    managerNames.removeAll("memory");

    foreach(QString mgr, managerNames) {
        QTest::newRow(QString("[%1]").arg(mgr).toLatin1().constData()) << mgr;
    }
}

/**
 * Adds event related test data for test case "addItem" for backend "managerName".
 */
void tst_SymbianOm::addItem_dataBasic(QString managerName, QString itemType)
{
    // The default detail list contains only the type (event)
    QTstDetailFieldList itemTypeDetails;
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    // There is a bug in Symbian calendar engine. It does not allow
    // creating an event without a start time.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName == m_managerNameSymbian
        && itemType == QOrganizerItemType::TypeEvent) {
        QTest::newRow(testCaseName("item without details", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::BadArgumentError // Symbian calendar server requires start time for an event
            << QTstDetailFieldList(itemTypeDetails);

        QTest::newRow(testCaseName("item with description", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::BadArgumentError // Symbian calendar server requires start time for an event
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));
    } else {
        QTest::newRow(testCaseName("item without details", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << QTstDetailFieldList(itemTypeDetails);

        QTest::newRow(testCaseName("item with description", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president")));
    }
}

/*!
 * Helper function for adding time rage related test data (QTest rows).
 * Note: the test cases are actually type dependent, because the time range
 * detail is type dependent.
 */
void tst_SymbianOm::addItem_dataEventTimeRange(QString managerName, QString itemType)
{
    // The default detail list contains only the type (event)
    QTstDetailFieldList itemTypeDetails = QTstDetailFieldList();
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    QTest::newRow(testCaseName("item with start time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));

    // There is a bug in symbian calendar server: An end datetime that is
    // exactly the same as the start datetime is ignored.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(testCaseName("item with start time = end time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime())
                << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime()));
    }

    QTest::newRow(testCaseName("item with start time < end time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));

    QTest::newRow(testCaseName("item with start time > end time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addSecs(3600))
            << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime()));

    // Symbian calendar server makes the client process panic in case the start
    // time of an event (calendar event of type EAppt) is left empty.
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        // This test case matches the iCalendar specification
        QTest::newRow(testCaseName("item with end time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    } else {
        // This test case is against the iCalendar specification
        QTest::newRow(testCaseName("item with end time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::BadArgumentError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerEventTimeRange::DefinitionName, QOrganizerEventTimeRange::FieldEndDateTime, QDateTime::currentDateTime().addSecs(3600)));
    }
}

void tst_SymbianOm::addItem_dataReminders(QString managerName, QString itemType)
{
    QTstDetailFieldList itemTypeDetails;
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    QString timeRangeDefinition = QOrganizerEventTimeRange::DefinitionName;
    QString fieldStartDateTime = QOrganizerEventTimeRange::FieldStartDateTime;
    QString fieldEndDateTime = QOrganizerEventTimeRange::FieldEndDateTime;

    if (itemType == QOrganizerItemType::TypeTodo) {
        timeRangeDefinition = QLatin1String(QOrganizerTodoTimeRange::DefinitionName);
        fieldStartDateTime = QLatin1String(QOrganizerTodoTimeRange::FieldStartDateTime);
        fieldEndDateTime = QLatin1String(QOrganizerTodoTimeRange::FieldDueDateTime);
    }

    QTest::newRow(testCaseName("item with reminder starttime", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // datetime reminder after half an hour
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addSecs(1800)));

    // There is a known issue in symbian calendar server, the datetimes seem to
    // be rounded to one day accuracy. In other words you cannot create for
    // example an ETodo CCalEntry with startDateTime after one hour and
    // dueDateTime after 24 hours. Also, you cannot create an ETodo CCalEntry
    // with only a startDateTime.
    // *** TODO: File a bug report against symbian calendar server in
    // Symbian S^3 and S^4!
    if (itemType != QOrganizerItemType::TypeTodo) {
        QTest::newRow(testCaseName("item with reminder starttime", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
                // start after an hour
                << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addSecs(3600))
                // datetime reminder after half an hour
                << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addSecs(1800)));
    }

    QTest::newRow(testCaseName("item with reminder starttime after 3 hours, 2 secs", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start now
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // datetime reminder ~3 hours ago
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addSecs(11802)));

    QTest::newRow(testCaseName("item with reminder starttime after 30 days", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start after 30 days
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(30))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(31))
            // datetime reminder after (29 days, 23 hours and 59 minutes)
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addSecs(2591940)));

    QTest::newRow(testCaseName("item with reminder starttime after 365 days", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start after 365 days
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(365))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(366))
            // datetime reminder after 364 days
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addDays(364)));

    QTest::newRow(testCaseName("item with reminder starttime in the past", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with the president"))
            // start now
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(1))
            // datetime reminder half an hour ago
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addSecs(-1800)));

    QTest::newRow(testCaseName("item with reminder delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // delta reminder half an hour before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldTimeDelta, int(1800)));

    QTest::newRow(testCaseName("item with small reminder delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // delta reminder 65 seconds before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldTimeDelta, int(65)));

    QTest::newRow(testCaseName("item with smallest reminder delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // delta reminder 1 second before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldTimeDelta, int(1)));

    QTest::newRow(testCaseName("item with reminder delta in past", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start now
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime())
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(1))
            // delta reminder half an hour before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldTimeDelta, int(-1800)));

    QTest::newRow(testCaseName("item with reminder datetime and delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // datetime reminder after 23,5 hours
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addDays(1).addSecs(-1800))
            // delta reminder half an hour before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldTimeDelta, int(1800)));

    QTest::newRow(testCaseName("item with conflicting reminder datetime and delta", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription, QString("Meeting with Elbonian president"))
            // start tomorrow
            << QTstDetailField(timeRangeDefinition, fieldStartDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(timeRangeDefinition, fieldEndDateTime, QDateTime::currentDateTime().addDays(2))
            // datetime reminder after half an hour
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldDateTime, QDateTime::currentDateTime().addSecs(1800))
            // delta reminder 15 minutes before
            << QTstDetailField(QOrganizerItemReminder::DefinitionName, QOrganizerItemReminder::FieldTimeDelta, int(900)));
}

/*!
 * Helper function for adding time rage related test data (QTest rows).
 * Note: the test cases are actually type dependent, because the time range
 * detail is type dependent.
 */
void tst_SymbianOm::addItem_dataTodoTimeRange(QString managerName, QString itemType)
{
    // The default detail list contains only the type (todo)
    QTstDetailFieldList itemTypeDetails;
    itemTypeDetails << QTstDetailField(
        QOrganizerItemType::DefinitionName,
        QOrganizerItemType::FieldType,
        itemType);

    // There is a bug in symbian calendar server that makes an empty
    // "not-before" datetime to be replaced with the same value as "due"
    // datetime if only "due" datetime was defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(testCaseName("item with due time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())); 
    }

    QTest::newRow(testCaseName("item with due time == not-before time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));

    QTest::newRow(testCaseName("item with due time > not-before time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime().addDays(1))
            << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime()));

    QTest::newRow(testCaseName("item with due time < not-before time", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime())
            << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addDays(1)));

    // There is a bug in symbian calendar server which makes the "not-before"
    // datetime to be ignored if no "due" datetime is defined
    // TODO: File a bug report against symbian calendar server in 10.1
    // platform. For older platforms this should be considered a known-issue
    // and test case should be left disabled.
    if (managerName != m_managerNameSymbian) {
        QTest::newRow(testCaseName("item with not-before time", managerName, itemType).toLatin1().constData())
            << managerName
            << (int) QOrganizerItemManager::NoError
            << (QTstDetailFieldList(itemTypeDetails)
                << QTstDetailField(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime().addDays(1)));
    }

    QTstDetailField timeStart(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldStartDateTime, QDateTime::currentDateTime());
    QTstDetailField timeDue(QOrganizerTodoTimeRange::DefinitionName, QOrganizerTodoTimeRange::FieldDueDateTime, QDateTime::currentDateTime().addDays(1));
    
    QTest::newRow(testCaseName("item with progress StatusNotStarted", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusNotStarted));
    
    QTest::newRow(testCaseName("item with progress StatusInProgress", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusInProgress));
    
    QTest::newRow(testCaseName("item with progress StatusComplete", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::NoError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete)
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldFinishedDateTime, QDateTime::currentDateTime()));    

    QTest::newRow(testCaseName("item with progress StatusComplete, no finished date", managerName, itemType).toLatin1().constData())
        << managerName
        << (int) QOrganizerItemManager::BadArgumentError
        << (QTstDetailFieldList(itemTypeDetails)
            << timeStart
            << timeDue
            << QTstDetailField(QOrganizerTodoProgress::DefinitionName, QOrganizerTodoProgress::FieldStatus, QOrganizerTodoProgress::StatusComplete));    
}

/*!
 * Helper function for generating test case name. The name is given in the
 * following format: "[managerName][itemType] testCase"
 */
QString tst_SymbianOm::testCaseName(QString testCase, QString managerName, QString itemType)
{
    return QString("[%1][%2] %3")
        .arg(managerName)
        .arg(itemType)
        .arg(testCase)
        .toLatin1().constData();
}

/*!
 * Helper function for parsing QTstDetailFields into organizer item details
 */
bool tst_SymbianOm::parseDetails(QTstDetailFieldList detailsList, QList<QOrganizerItemDetail> &details)
{
    foreach (QTstDetailField tstdetail, detailsList) {
        QOrganizerItemDetail detail(tstdetail.m_definitionName);
        int existingIndex(-1);
        foreach (QOrganizerItemDetail existingDetail, details) {
            if (existingDetail.definitionName() == detail.definitionName()) {
                // Replace the existing detail in the list so that the new
                // field is added to the existing detail
                // TODO: check that the field does not exist already
                existingIndex = details.indexOf(existingDetail);
                detail = existingDetail;
            }
        }

        if (detail.setValue(tstdetail.m_fieldName, tstdetail.m_value)) {
            // If a detail with same name exists, replace
            if (existingIndex != -1) {
                details.replace(existingIndex, detail);
            } else {
                details.append(detail);
            }
        } else {
            qDebug() << QString("Failed to set field value!");
            return false;
        }
    }
    return true;
}

/*!
 * Helper function for verifying details against reference details
 */
bool tst_SymbianOm::verifyDetails(QList<QOrganizerItemDetail> actual, QList<QOrganizerItemDetail> expected)
{
	foreach (QOrganizerItemDetail expectedDetail, expected) {
        bool found(false);
        foreach (QOrganizerItemDetail actualDetail, actual) {

            if (actualDetail.definitionName() == expectedDetail.definitionName()) {
                // Compare 
                if (actualDetail.variantValues() == expectedDetail.variantValues()) {
                    // A valid detail with the same field values found
                    found = true;
                    break;
                } else {
                    // Check if this is one of the known issues
                    if (isKnownIssue(actualDetail, expectedDetail)) {
                        found = true;
                    } else {
                        qDebug() << "Detail value mismatch " << expectedDetail.definitionName();
                        qDebug() << "Actual  :" << actualDetail.variantValues();
                        qDebug() << "Expected:" << expectedDetail.variantValues();
                        return false;
                    }
                }
            }
        }

        if (!found) {
            qDebug() << "Expected detail not found:" << expectedDetail.definitionName();
            return false;
        }
    }

    return true;
}

bool tst_SymbianOm::isKnownIssue(QOrganizerItemDetail actual, QOrganizerItemDetail expected)
{
    if(actual.definitionName() == QOrganizerItemReminder::DefinitionName
        && expected.definitionName() == QOrganizerItemReminder::DefinitionName) {
        QOrganizerItemReminder actualReminder = actual;
        QOrganizerItemReminder expectedReminder = expected;

        // Known issue:
        // DateTime and Delta fields of a reminder depend on each
        // other. If only DateTime is defined, Delta is calculated
        // by symbian backend and vice-versa.
        if (expectedReminder.dateTime().isValid()
            && expectedReminder.dateTime() == actualReminder.dateTime()
            && expectedReminder.timeDelta() == 0
            && actualReminder.timeDelta() != 0) {
            // The expected detail contains startDateTime but not delta
            // -> Ignore delta field in actual detail.
            qWarning() << "Known issue 1";
            return true;
        } else if (expectedReminder.dateTime().isNull()
            && actualReminder.dateTime().isValid()
            && expectedReminder.timeDelta() != 0
            && expectedReminder.timeDelta() == actualReminder.timeDelta()) {
            // The expected detail contains delta but not startDateTime
            // -> Ignore datetime field in actual detail.
            qWarning() << "Known issue 2";
            return true;
        }

        // Known issue:
        // Symbian calendar server saves reminder delta as minutes.
        // QOrganizerItemReminder::FieldTimeDelta uses seconds.
        // This causes a rounding issue.
        if(expectedReminder.timeDelta()
            && actualReminder.timeDelta() != expectedReminder.timeDelta()
            && actualReminder.timeDelta() == ((expectedReminder.timeDelta() / 60) * 60)) {
            qWarning() << "Known issue 3";
            return true;
        } else if(actualReminder.dateTime().isValid()
            && expectedReminder.dateTime().isValid()
            && actualReminder.dateTime() != expectedReminder.dateTime()
            && ((actualReminder.dateTime().toTime_t() / 60) * 60)
            == ((expectedReminder.dateTime().toTime_t() / 60) * 60)) {
            qWarning() << "Known issue 4";
            return true;
        }
    }

    return false;
}

bool tst_SymbianOm::compareItemLists(QList<QOrganizerItem> actual, QList<QOrganizerItem> expected, QStringList ingoredDetails)
{
    // Remove matching contacts
    foreach (QOrganizerItem a, actual) {
        foreach (QOrganizerItem b, expected) {
            if (compareItems(a, b, ingoredDetails)) {
                actual.removeOne(a);
                expected.removeOne(b);
                break;
            }
        }
    }
    
    if (actual.count()) {
        qDebug() << "Extra items:";
        qDebug() << actual;
    }
    
    if (expected.count()) {
        qDebug() << "Missing items:";
        qDebug() << expected;
    }    
    
    if (actual.count() == 0 && expected.count() == 0)
        return true;
    
    return false;
}

bool tst_SymbianOm::compareItems(QOrganizerItem itemA, QOrganizerItem itemB, QStringList ignoredDetails)
{
    if (itemA.localId() != itemB.localId())
        return false;
    
    QList<QOrganizerItemDetail> aDetails = itemA.details();
    QList<QOrganizerItemDetail> bDetails = itemB.details();
    
    // Remove ignored details
    foreach (QString ignored, ignoredDetails) { 
        foreach (QOrganizerItemDetail d, aDetails) {
            if (d.definitionName() == ignored)
                itemA.removeDetail(&d);
        }
    }
    
    foreach (QString ignored, ignoredDetails) { 
        foreach (QOrganizerItemDetail d, bDetails) {
            if (d.definitionName() == ignored)
                itemB.removeDetail(&d);
        }
    }
    
    // They can be in any order, so loop
    foreach (QOrganizerItemDetail d, aDetails) {
        foreach (QOrganizerItemDetail d2, bDetails) {
            if (d == d2) {
                itemA.removeDetail(&d);
                itemB.removeDetail(&d2);
                break;
            }
        }
    }
    
    if (itemA == itemB)
        return true;
    
    return false;
}

QTEST_MAIN(tst_SymbianOm);
#include "tst_symbianom.moc"