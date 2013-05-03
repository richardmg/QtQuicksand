/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "ContactsDialog.h"
#include <QHBoxLayout>
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QLabel>

ContactsDialog::ContactsDialog(QContactManager* manager, QWidget *parent) :
    QDialog(parent)
{
    m_contactManager = manager;
    
    QVBoxLayout* vbox = new QVBoxLayout;
    QHBoxLayout* hbox = new QHBoxLayout;

    QLabel* label = new QLabel("Send MMS to",this);
    label->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    vbox->addWidget(label, Qt::AlignHCenter | Qt::AlignVCenter);

    // List
    m_listWidget = new QListWidget(this);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vbox->addWidget(m_listWidget);


    // Buttons
    m_ok = new QPushButton("Select", this);
    m_ok->setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(m_ok, SIGNAL(clicked()), this, SLOT(selectContact()));
    hbox->addWidget(m_ok, Qt::AlignTop | Qt::AlignVCenter);

    m_exit = new QPushButton("Cancel", this);
    m_exit->setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(m_exit, SIGNAL(clicked()), this, SLOT(close()));
    hbox->addWidget(m_exit, Qt::AlignTop | Qt::AlignVCenter);

    vbox->addLayout(hbox);


    // Remove context menu from the all widgets
#ifdef Q_OS_SYMBIAN
    QWidgetList widgets = QApplication::allWidgets();
    QWidget* w = 0;
    foreach(w,widgets)
        {
            w->setContextMenuPolicy(Qt::NoContextMenu);
        }
#endif

    // Set dialog title for the Maemo
#if defined Q_WS_MAEMO_5
    this->setWindowTitle("Select contact");
#endif

    // Use layout
    this->setLayout(vbox);

    // Create QContactManager and search contacts
    QTimer::singleShot(0,this,SLOT(findContacts()));
    
}

ContactsDialog::~ContactsDialog()
{
    delete m_listWidget;
}

void ContactsDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
}

void ContactsDialog::findContacts()
{
    searchContact();
}

void ContactsDialog::searchContact()
{
    m_listWidget->clear();

    // Sort contacts by lastname
    QContactSortOrder sort;
    sort.setDirection(Qt::AscendingOrder);
    sort.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);

    // Build QListWidget from the contact list
    QList<QContactLocalId> contactIds = m_contactManager->contactIds(sort);
    QContact currContact;
    foreach (const QContactLocalId& id, contactIds)
        {
            QListWidgetItem *currItem = new QListWidgetItem;
            currContact = m_contactManager->contact(id);
            QContactDisplayLabel dl = currContact.detail(QContactDisplayLabel::DefinitionName);
            currItem->setData(Qt::DisplayRole, dl.label());
            currItem->setData(Qt::UserRole, currContact.localId()); // also store the id of the contact
            m_listWidget->addItem(currItem);
        }
}

void ContactsDialog::selectContact()
{
    QList<QListWidgetItem*> items = m_listWidget->selectedItems();
    if (!items.isEmpty()) {
        itemClicked(items.first());
    }
}

void ContactsDialog::itemClicked(QListWidgetItem *item)
{
    QVariant data = item->data(Qt::UserRole);
    QContactLocalId id = data.toInt();
    QContact contact = m_contactManager->contact(id);
    QContactPhoneNumber cpn = contact.detail<QContactPhoneNumber> ();

    if (!cpn.isEmpty()) {
        emit contactSelected(cpn.number());
        close();
    } else {
        close();
    }
}

