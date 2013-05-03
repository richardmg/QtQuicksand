/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef CONTACTSDIALOG_H
#define CONTACTSDIALOG_H

#include <QtGui/QDialog>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

// QtMobility API headers
// Contacts
#include <QContactManager>
#include <QContactPhoneNumber>
#include <QContactSortOrder>
#include <QContact>
#include <QContactName>

// QtMobility namespace
QTM_USE_NAMESPACE


class ContactsDialog: public QDialog
{
Q_OBJECT

public:
    ContactsDialog(QContactManager* manager, QWidget *parent = 0);
    virtual ~ContactsDialog();
    void resizeEvent(QResizeEvent *);
    
public slots:
    void findContacts();
    void itemClicked(QListWidgetItem *item);
    void selectContact();

signals:
    void contactSelected(QString phoneNumber);

private:
    void searchContact();

protected:
    QPointer<QContactManager> m_contactManager;
    QPointer<QListWidget> m_listWidget;
    QPushButton* m_ok;
    QPushButton* m_exit;
};

#endif // CONTACTSDIALOG_H
