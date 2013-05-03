/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QtGui>
#include <QTimeLine>
#include <QProcess>
#include "toolbarbutton.h"
#include "vsandbox.h"
#include "vsand_intro.h"
//ANDROID
//sensors
#if defined Q_OS_ANDROID
#include <QAccelerometer>
#include <QAccelerometerReading>
#endif
// Qt Mobility
#if defined Q_WS_MAEMO_5
#include <QAccelerometer>
#include <QAccelerometerReading>
#include <QContactManager>
#include <QMessage>
#include <QMessageService>
QTM_USE_NAMESPACE
#endif

#if defined Q_OS_SYMBIAN
#include <QAccelerometer>
#include <QAccelerometerReading>
#include <QContactManager>
#include <QMessage>
#include <QMessageService>
#include "CameraFileObserver.h"
QTM_USE_NAMESPACE
#endif

#if defined Q_WS_MAEMO_5
#include <QtDBus/QtDBus>
#endif

class ToolBarWidget;
class ContactsDialog;
#ifdef Q_OS_SYMBIAN
class Widget: public QWidget, MCameraFileObserver
#else
class Widget: public QWidget
#endif
{
Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

    bool event(QEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

public slots:
    void pressed(ToolBarButton::ButtonType);
    void startToolbarAnimation();
    void animate(qreal);
    void readSensor();
    void sendMMS(QString phoneNumber);
    void listenCamera();
    void readCameraImage();

#ifdef Q_OS_SYMBIAN
    void NewCameraFileL(const TFileName& aFileName);
#endif


private:
    void paintToPixmap();
    void timerEvent(QTimerEvent *);
    void newSandFrame();        // run sandframe and draw the result into member image.
    void toolStep( int fixedx, int fixedy, int ti, int *id );
    void startCamera();
    void autopaint( QImage target );

private:
    QPoint p1;
    QPoint p2;

    QImage image;

    QSize screenSize;
    QPen pen;
    QPen draftPen;
    QColor color;

    QPointer<QMessageBox> messageBox;
    ToolBarWidget* toolbar;
    int                 m_toolbar_tid;
    int                 m_toolBarVisibilityCounter;

    QPointer<QTimeLine> timeLine;

#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN
    QPointer<ContactsDialog> m_contactsDlg;
    QContactManager*         m_contactManager;
    QMessageService*         m_messageService;
#endif

    int                 m_fastUpdateTimer;

#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN || defined Q_OS_ANDROID
    QAccelerometer*     m_sensor;
    int                 m_sensor_tid;
    QString             m_string;
#endif

    int                 m_callResetInactivity;
    int                 m_inactivityCounter;

    // intro specific
    CVSand_Intro*       m_intro;

    // SandBox-specific
    float               m_tangle;
    CVSandBox*          m_sandBox;
    eSANDTOOL           m_selectedTool;// tool from toolbar
    eSANDTOOL           m_selectedSandTool;// tool from adding/removing sand for toolbar

    int                 m_touchCounter;
    int                 m_paintingPosX[3];
    int                 m_paintingPosY[3];
    int                 m_expaintingPosX[3];
    int                 m_expaintingPosY[3];
    eSANDTOOL           m_paintingTool[3];
    int                 m_paintingSize;

    bool                m_touchEnabled;

    QProcess*           m_cameraProcess;

    QFont               m_font;

    QString             m_pathToImage;

#ifdef Q_OS_SYMBIAN
    CCameraFileObserver* m_cameraFileObserver;
#endif

    QMainWindow*        m_mainWindow;
};

#endif // WIDGET_H
