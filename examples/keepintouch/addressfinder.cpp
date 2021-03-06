/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "addressfinder.h"

#include <qcontactdetailfilter.h>
#include <qcontactemailaddress.h>
#include <qcontactmanager.h>
#include <qcontactphonenumber.h>
#include <qmessage.h>
#include <qmessageservice.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QMenuBar>
#include <QTabWidget>
#include <QScrollArea>

namespace {

QString simpleAddress(const QString &recipient)
{
    QString name;
    QString addressOnly;

    QMessageAddress::parseEmailAddress(recipient, &name, &addressOnly);

    return addressOnly;
}

QString simpleAddress(const QMessageAddress &address)
{
    return simpleAddress(address.addressee());
}

//! [contact-lookup]
QString contactDisplayName(const QMessageAddress &address)
{
    QString addressOnly(simpleAddress(address));

    // See if we can match this address to a contact
    QContactDetailFilter filter;
    if (address.type() == QMessageAddress::Email) {
        // Match contacts on email address data
        filter.setDetailDefinitionName(QContactEmailAddress::DefinitionName, QContactEmailAddress::FieldEmailAddress);
        filter.setValue(addressOnly);
        filter.setMatchFlags(QContactFilter::MatchContains);
    } else if (address.type() == QMessageAddress::Phone) {
        // Match contacts on phone number data
        filter.setDetailDefinitionName(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber);
        filter.setValue(addressOnly);
        filter.setMatchFlags(QContactFilter::MatchPhoneNumber);
    }

    QContactManager manager;
    foreach (const QContactLocalId &contactId, manager.contactIds(filter)) {
        // Any match is acceptable
        const QContact &contact(manager.contact(contactId));
        return contact.displayLabel();
    }

    // We couldn't match anything, so return the original address
    return address.addressee();
}
//! [contact-lookup]

}

AddressFinder::AddressFinder(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
      tabWidget(0),
      includePeriod(0),
      excludePeriod(0),
      excludeCheckBox(0),
      searchAction(0),
      searchButton(0),
      contactList(0),
      messageCombo(0)
 {
    setupUi();

    connect(&service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));
    connect(&service, SIGNAL(messagesFound(QMessageIdList)), this, SLOT(messagesFound(QMessageIdList)));
}

AddressFinder::~AddressFinder()
{
}

void AddressFinder::includePeriodChanged(int selected)
{
    // Only allow smaller periods to be excluded
    excludePeriod->clear();

    switch (selected) {
        case 0: excludePeriod->insertItem(0, "9 Months"); // fall through:
        case 1: excludePeriod->insertItem(0, "6 Months");
        case 2: excludePeriod->insertItem(0, "3 Months");
        case 3: excludePeriod->insertItem(0, "Month");
        case 4: excludePeriod->insertItem(0, "Week");
        default: break;
    }

    excludePeriod->setCurrentIndex(0);
}

void AddressFinder::excludePeriodEnabled(int state)
{
    excludePeriod->setEnabled(state == Qt::Checked);
}

//! [address-selected]
void AddressFinder::addressSelected(int index)
{
    messageCombo->clear();

    if (index == -1)
        return;

#ifdef USE_CONTACTS_COMBOBOX
    QString address = contactList->itemData(index).toString();
#else
    QString address = contactList->item(index)->data(Qt::UserRole).toString();
#endif

    QString addressOnly(simpleAddress(address));

    // Add the subject of each message to this address to the message pane
    typedef QPair<QString, QMessageId> MessageDetails;
    foreach (const MessageDetails &message, addressMessages[addressOnly]) {
        messageCombo->addItem(message.first);
    }
}
//! [address-selected]

void AddressFinder::searchMessages()
{
    setSearchActionEnabled(false);

    contactList->clear();
    messageCombo->clear();
    excludedAddresses.clear();
    addressList.clear();
    addressMessages.clear();
    inclusionMessages.clear();
    exclusionMessages.clear();

//! [create-date-range]
    QDateTime now(QDateTime::currentDateTime());
    bool useExclusionPeriod(excludeCheckBox->isChecked());

    // Determine the dates that demarcate the selected range
    QDateTime minimumDate(now);
    switch (includePeriod->currentIndex()) {
        case 0: minimumDate = minimumDate.addMonths(-12); break;
        case 1: minimumDate = minimumDate.addMonths(-9); break;
        case 2: minimumDate = minimumDate.addMonths(-6); break;
        case 3: minimumDate = minimumDate.addMonths(-3); break;
        case 4: minimumDate = minimumDate.addMonths(-1); break;
        case 5: minimumDate = minimumDate.addDays(-7); break;
        default: break;
    }

    QDateTime maximumDate(now);
    if (useExclusionPeriod) {
        // We have an exclusion period to apply
        switch (excludePeriod->currentIndex()) {
            case 0: maximumDate = maximumDate.addDays(-7); break;
            case 1: maximumDate = maximumDate.addMonths(-1); break;
            case 2: maximumDate = maximumDate.addMonths(-3); break;
            case 3: maximumDate = maximumDate.addMonths(-6); break;
            case 4: maximumDate = maximumDate.addMonths(-9); break;
            default: break;
        }
    }
//! [create-date-range]

//! [create-simple-filters]
    // We will include addresses contacted following the minimum date
    QMessageFilter includeFilter(QMessageFilter::byTimeStamp(minimumDate, QMessageDataComparator::GreaterThanEqual));
    // Windows mobile only sets a receptionTimeStamp for sent messsages
    includeFilter |= QMessageFilter::byReceptionTimeStamp(minimumDate, QMessageDataComparator::GreaterThanEqual);

    QMessageFilter excludeFilter;
    if (useExclusionPeriod) {
        // We will exclude addresses contacted following the maximum date
        excludeFilter = QMessageFilter::byTimeStamp(maximumDate, QMessageDataComparator::GreaterThanEqual);
        excludeFilter |= QMessageFilter::byReceptionTimeStamp(maximumDate, QMessageDataComparator::GreaterThanEqual);
    }
//! [create-simple-filters]

//! [create-composite-filters]
    // We only want to match messages that we sent
    QMessageFilter sentFilter(QMessageFilter::byStandardFolder(QMessage::SentFolder));

    // Create the filter needed to locate messages to search for addresses to include
    if (useExclusionPeriod) {
        inclusionFilter = (sentFilter & includeFilter & ~excludeFilter);
    } else {
        inclusionFilter = (sentFilter & includeFilter);
    }
//! [create-composite-filters]

//! [begin-search]
    if (useExclusionPeriod) {
        // Create the filter needed to locate messages whose address we will exclude
        QMessageFilter exclusionFilter;
        exclusionFilter = (sentFilter & excludeFilter);
        
        // Start the search for messages containing addresses to exclude
        service.queryMessages(exclusionFilter);
    } else {
        // Only search for messages containing addresses to include
        service.queryMessages(inclusionFilter);

        // Clear the inclusion filter to indicate that we have searched for it
        inclusionFilter = QMessageFilter();
    }
//! [begin-search]
}

//! [handle-search-result]
void AddressFinder::stateChanged(QMessageService::State newState)
{
    if (searchAction->isEnabled() == false) {
        if (newState == QMessageService::FinishedState) {
            if (service.error() == QMessageManager::NoError) {
                if (!inclusionFilter.isEmpty()) {
                    // Now find the included messages
                    service.queryMessages(inclusionFilter);

                    // Clear the inclusion filter to indicate that we have searched for it
                    inclusionFilter = QMessageFilter();
                } else {
                    // We have found the exclusion and inclusion message sets
                    if (!inclusionMessages.isEmpty()) {
                        // Begin processing the message sets
                        QTimer::singleShot(0, this, SLOT(continueSearch()));
//! [handle-search-result]
                    } else {
                        QMessageBox::information(0, tr("Empty"), tr("No messages found"));
                        setSearchActionEnabled(true);
                    }
                }
            } else {
                QMessageBox::warning(0, tr("Failed"), tr("Unable to perform search"));
                setSearchActionEnabled(true);
            }
        }
    }
}

//! [accumulate-matches]
void AddressFinder::messagesFound(const QMessageIdList &ids)
{
    // Add these IDs to the relevant set
    if (!inclusionFilter.isEmpty()) {
        exclusionMessages << ids;
    } else {
        inclusionMessages << ids;
    }
}
//! [accumulate-matches]

//! [continue-search]
void AddressFinder::continueSearch()
{
    if (!exclusionMessages.isEmpty()) {
        // Take the first message whose addreses we should exclude
        QMessageId id(exclusionMessages.takeFirst());
        const QMessage message(id);

        // All recipient addresses are to be excluded
        foreach (const QMessageAddress &address, message.to() + message.cc() + message.bcc()) {
            excludedAddresses.insert(simpleAddress(address));
        }
    } else if (!inclusionMessages.isEmpty()) {
        // Take the first message to inspect for suitable addresses
        QMessageId id(inclusionMessages.takeFirst());
        const QMessage message(id);

        QString details;

        // For each recipient of this message
        foreach (const QMessageAddress &address, message.to() + message.cc() + message.bcc()) {
            QString addressOnly(simpleAddress(address));

            // Ignore recipients whose addresses we have added to the exclusion set
            if (!excludedAddresses.contains(addressOnly)) {
                // Link this message to this address
                QList<QPair<QString, QMessageId> > &messageList(addressMessages[addressOnly]);
                if (messageList.isEmpty()) {
                    addressList.append(addressOnly);

                    // Add the recipient to our visible list of contacts to keep in touch with
#ifdef USE_CONTACTS_COMBOBOX
                    contactList->addItem(contactDisplayName(address), addressOnly);
#else
                    QListWidgetItem* newListWidgetItem = new QListWidgetItem(contactDisplayName(address));
                    newListWidgetItem->setData(Qt::UserRole, addressOnly);
                    contactList->addItem(newListWidgetItem);
#endif
                }

                if (details.isEmpty()) {
                    // Determine the properties of the message
                    details = QString("[%1] %2").arg(message.date().toString("MMM d")).arg(message.subject());
                }
                messageList.append(qMakePair(details, id));
            }
        }
    }

    if (!exclusionMessages.isEmpty() || !inclusionMessages.isEmpty()) {
        // There are more messages to process
        QTimer::singleShot(0, this, SLOT(continueSearch()));
    } else {
        // We're finished our search
        setSearchActionEnabled(true);
#ifndef USE_SEARCH_BUTTON
        tabChanged(1);
#endif

#ifdef USE_CONTACTS_COMBOBOX
        addressSelected(contactList->currentIndex());
#else
        addressSelected(contactList->currentRow());
#endif
    }
}
//! [continue-search]

#ifndef USE_SEARCH_BUTTON
void AddressFinder::tabChanged(int index)
{
    QWidget* currentTab = tabWidget->currentWidget();
    QAction* action = 0;
    if(currentTab && !currentTab->actions().isEmpty())
        action = currentTab->actions().first();
    menuBar()->setDefaultAction(action);

    Q_UNUSED(index)
}
#endif

void AddressFinder::setupUi()
{
    setWindowTitle(tr("Keep In Touch"));

#ifndef USE_SEARCH_BUTTON
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int)));
#else
    QWidget* centralWidget = new QWidget(this);
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(centralWidget);
    scrollArea->setWidgetResizable(true);
    setCentralWidget(scrollArea);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);
#endif

    QGroupBox *inputGroup = new QGroupBox(tr("Find addresses"));
    inputGroup->setAlignment(Qt::AlignLeft);
#ifndef USE_SEARCH_BUTTON
    tabWidget->addTab(inputGroup,"Search");
#else
    centralLayout->addWidget(inputGroup);
#endif

    QGridLayout *filterLayout = new QGridLayout(inputGroup);
#ifdef Q_WS_MAEMO_5
    // Maemo 5 style doesn't take group box titles into account.
    int spacingHack = QFontMetrics(QFont()).height();
    filterLayout->setContentsMargins(0, spacingHack, 0, 0);
#endif

    QLabel *includeLabel = new QLabel(tr("Contacted in the last"));
    filterLayout->addWidget(includeLabel, 0, 0);
    filterLayout->setAlignment(includeLabel, Qt::AlignRight);

    excludeCheckBox = new QCheckBox(tr("But not in the last"));
#ifdef Q_WS_MAEMO_5
    // Maemo 5 style cuts off check box text.
    excludeCheckBox->setText(excludeCheckBox->text() + "  ");
#endif
    connect(excludeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(excludePeriodEnabled(int)));
    filterLayout->addWidget(excludeCheckBox, 1, 0);
    filterLayout->setAlignment(excludeCheckBox, Qt::AlignRight);

    includePeriod = new QComboBox;
    includePeriod->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    includePeriod->addItem(tr("Year"));
    includePeriod->addItem(tr("9 Months"));
    includePeriod->addItem(tr("6 Months"));
    includePeriod->addItem(tr("3 Months"));
    includePeriod->addItem(tr("Month"));
    includePeriod->addItem(tr("Week"));
    connect(includePeriod, SIGNAL(currentIndexChanged(int)), this, SLOT(includePeriodChanged(int)));
    filterLayout->addWidget(includePeriod, 0, 1);

    excludePeriod = new QComboBox;
    excludePeriod->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filterLayout->addWidget(excludePeriod, 1, 1);
    excludePeriod->setEnabled(false);

#ifdef USE_SEARCH_BUTTON
    searchButton = new QPushButton(tr("Search"));
    searchButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(searchMessages()), Qt::QueuedConnection);
    filterLayout->addWidget(searchButton, 2, 1);
#endif

#ifdef USE_CONTACTS_COMBOBOX
    contactList = new QComboBox(this);
    connect(contactList, SIGNAL(currentIndexChanged(int)), this, SLOT(addressSelected(int)));
#else
    contactList = new QListWidget(this);
    connect(contactList, SIGNAL(currentRowChanged(int)), this, SLOT(addressSelected(int)));
#endif

#ifndef USE_SEARCH_BUTTON
    QWidget* resultsWidget = new QWidget(this);
    QVBoxLayout* resultsLayout = new QVBoxLayout(resultsWidget);
    tabWidget->addTab(resultsWidget,"Results");
#else
    QVBoxLayout* resultsLayout = centralLayout;
#endif

    QGroupBox *addressGroup = new QGroupBox(tr("Contacts"));
    addressGroup->setAlignment(Qt::AlignLeft);
    addressGroup->setLayout(new QVBoxLayout);
#ifdef Q_WS_MAEMO_5
    addressGroup->layout()->setContentsMargins(0, spacingHack, 0, 0);
#endif
    addressGroup->layout()->addWidget(contactList);
    resultsLayout->addWidget(addressGroup);

    QGroupBox *messageGroup = new QGroupBox(tr("Messages"));
    messageGroup->setAlignment(Qt::AlignLeft);

    QVBoxLayout *groupLayout = new QVBoxLayout;
#ifdef Q_WS_MAEMO_5
    groupLayout->setContentsMargins(0, spacingHack, 0, 0);
#endif

    messageCombo = new QComboBox;
    connect(messageCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(messageIndexChanged(int)));

    groupLayout->addWidget(messageCombo);

    showButton = new QPushButton(tr("Show..."));
    showButton->setEnabled(false);
    connect(showButton, SIGNAL(clicked()), this, SLOT(showMessage()));

    forwardButton = new QPushButton(tr("Forward..."));
    forwardButton->setEnabled(false);
    connect(forwardButton, SIGNAL(clicked()), this, SLOT(forwardMessage()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(showButton);
    buttonLayout->addWidget(forwardButton);

    groupLayout->addLayout(buttonLayout);

    messageGroup->setLayout(groupLayout);
    resultsLayout->addWidget(messageGroup);

    searchAction = new QAction("Search",this);
    inputGroup->addAction(searchAction);
    connect(searchAction,SIGNAL(triggered()),this,SLOT(searchMessages()),Qt::QueuedConnection);
    QAction* quitAction = menuBar()->addAction("Quit");
    connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));

    includePeriodChanged(0);
#ifndef USE_SEARCH_BUTTON
    tabChanged(0);
#endif

    QWidgetList focusableWidgets;
    focusableWidgets << excludeCheckBox
                     << includePeriod
                     << excludePeriod
                     << contactList
                     << messageCombo
                     << showButton
#ifndef USE_SEARCH_BUTTON
                     << tabWidget
#else
                     << searchButton
                     << scrollArea
#endif
                     << forwardButton;

  foreach(QWidget* w, focusableWidgets)
       w->setContextMenuPolicy(Qt::NoContextMenu);

    includePeriod->setFocus();
}

void AddressFinder::setSearchActionEnabled(bool val)
{
    searchAction->setEnabled(val);
#ifdef USE_SEARCH_BUTTON
    if (val)
        searchButton->setText(tr("Search"));
    else
        searchButton->setText(tr("Searching.."));
    searchButton->setEnabled(val);
#endif
}

void AddressFinder::messageIndexChanged(int index)
{
    bool messageSelected(index != -1);
    showButton->setEnabled(messageSelected);
    forwardButton->setEnabled(messageSelected);
}

//! [show-message]
void AddressFinder::showMessage()
{
    int index = messageCombo->currentIndex();
    if (index != -1) {
        // Find the address currently selected
        const QString &selectedAddress(addressList[
#ifdef USE_CONTACTS_COMBOBOX
                contactList->currentIndex()
#else
                contactList->currentRow()
#endif
                ]);

        // Show the message selected
        QMessageId &messageId((addressMessages[selectedAddress])[index].second);
        service.show(messageId);
    }
}
//! [show-message]

//! [compose-message]
void AddressFinder::forwardMessage()
{
    int index = messageCombo->currentIndex();
    if (index != -1) {
        // Find the address currently selected
        const QString &selectedAddress(addressList[
#ifdef USE_CONTACTS_COMBOBOX
                contactList->currentIndex()
#else
                contactList->currentRow()
#endif
                ]);

        // Find the selected message
        QMessageId &messageId((addressMessages[selectedAddress])[index].second);
        QMessage original(messageId);

        // Create a message which forwards the selected message to the same recipient
        QMessage fwd(original.createResponseMessage(QMessage::Forward));
        fwd.setTo(original.to());
        service.compose(fwd);
    }
}
//! [compose-message]

