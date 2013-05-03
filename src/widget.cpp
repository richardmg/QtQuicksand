/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include <math.h>
#include "widget.h"
#include "toolbarwidget.h"
#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>

#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN
#include "ContactsDialog.h"
#endif

#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#endif

// 30 = N
const char *copyright_text =
    "This program is developed for Nokia World 2010: Hackathon"
    "By Tuomo Hirvonen and Tero Paananen";




Widget::Widget(QWidget *p) :
    QWidget(p)
{

    m_mainWindow = qobject_cast<QMainWindow*>(p);

    m_callResetInactivity = 0;
    m_cameraProcess = 0;
    m_touchCounter = 0;
#ifdef Q_OS_SYMBIAN
    m_cameraFileObserver = CCameraFileObserver::NewL(*this);
#endif

    setAttribute(Qt::WA_AcceptTouchEvents);
    p1 = QPoint(-1, -1);
    p2 = QPoint(-1, -1);
    m_toolbar_tid = 0;
    m_touchEnabled = false;

#if defined Q_WS_MAEMO_5
    m_contactManager = new QContactManager("maemo5");
    m_messageService = new QMessageService(this);
#elif defined Q_OS_SYMBIAN
    m_contactManager = new QContactManager("symbian");
    m_messageService = new QMessageService(this);
#endif


    QString f = m_font.defaultFamily();
    m_font.setFamily(f);

    // Create paper
    QSize s = QApplication::desktop()->size();
    image = QImage(s,QImage::Format_RGB32 );

    // Create default pens
    color = QColor(Qt::black);
    pen.setColor(color);
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    draftPen.setColor(Qt::black);
    draftPen.setStyle(Qt::DashLine);
    draftPen.setWidth(1);

    // Create custom toolbar
    toolbar = new ToolBarWidget(this);
    toolbar->setDownButton(ToolBarButton::ESnow);
    m_selectedTool = eSANDTOOL_PUSH_TOWARDS;

    pen.setWidth(toolbar->penSize());
    QObject::connect(toolbar, SIGNAL(pressed(ToolBarButton::ButtonType)), this,
        SLOT(pressed(ToolBarButton::ButtonType)));

    m_inactivityCounter = 0;
    m_tangle = 0.0f;

    // startup the sandSimulation
    m_sandBox = new CVSandBox( image.width(), image.height());
    m_sandBox->gravity().set(0,0);          // no gravity at the begining
    m_sandBox->setLight( FP_VAL*200, FP_VAL*200 );

    for (int i=0; i<3; i++) {
        m_paintingTool[i] =  eSANDTOOL_NONE;            // OFF
    }
    m_paintingSize = toolbar->penSize();


    // startup the timer for simulation
    // UI update timer (ms)
    m_fastUpdateTimer = 0;
    m_fastUpdateTimer = this->startTimer(16);           // max - framerate of 65535

    m_toolbar_tid = this->startTimer(100);
    m_toolBarVisibilityCounter = 0;

    // Sensor and reading timer (ms)
#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN  ||defined Q_OS_ANDROID
    m_sensor = new QAccelerometer(this);
    m_sensor->start();
    m_sensor_tid = 0;
    m_sensor_tid = this->startTimer(100);
#endif

    m_sandBox->autoPaintToResetTarget( 6, 0 );
    m_intro = new CVSand_Intro(false);

    if (strcmp( copyright_text, "justuse") == 0) {
        delete m_intro;
    };
}


Widget::~Widget()
{
    if (m_intro) delete m_intro;
    if (m_sandBox) delete m_sandBox;
    if (m_cameraProcess) delete m_cameraProcess;

#if defined Q_OS_SYMBIAN
    delete m_cameraFileObserver;
#endif

#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN
    m_sensor->stop();
    delete m_contactManager;
#endif
#if defined Q_OS_ANDROID
    m_sensor->stop();
#endif
}

void Widget::autopaint( QImage target )
{
    CTSurface_RGBA8888 apt;
    apt.setAsReference( target.bits(), target.width(), target.height(), target.bytesPerLine()/4, eTSURFACEFORMAT_RGBA8888 );
    m_sandBox->autoPaintTo( apt  );
}


void Widget::toolStep( int fixedx, int fixedy, int ti, int *id )
{
    switch (m_paintingTool[ti]) {
    default:
        break;

    case eSANDTOOL_SCRATCH_TO_TARGET:
        m_sandBox->getGrid()->scratch_to_target(fixedx,fixedy, (m_paintingSize+3)*FP_VAL, FP_VAL/2, m_sandBox->getAutoPaintTargetMem(),  id);
        break;

    case eSANDTOOL_ADD:
        {
            m_sandBox->getGrid()->addMass( fixedx,fixedy, (m_paintingSize+4)*FP_VAL, FP_VAL/16, id);
        }
        break;
    case eSANDTOOL_CURVE:
        m_sandBox->getGrid()->curveMass( fixedx,fixedy, (m_paintingSize)*FP_VAL, FP_VAL);
        break;
    case eSANDTOOL_PUSH:
        m_sandBox->getGrid()->push( fixedx,fixedy, (m_paintingSize+1)*FP_VAL, FP_VAL*2, id);
        break;

    case eSANDTOOL_PUSH_TOWARDS:
        m_sandBox->getGrid()->push_towards( fixedx, fixedy,
                                            id[0], id[1],(m_paintingSize+3)*FP_VAL, FP_VAL*2);
        break;


    case  eSANDTOOL_PULL:
        m_sandBox->getGrid()->pull( fixedx +(int)(cosf(m_tangle)*(20000.0f*(float)m_paintingSize)),
                                    fixedy +(int)(sinf(m_tangle)*(20000.0f*(float)m_paintingSize)),
                                    m_paintingSize*FP_VAL*2, FP_VAL*3/2);
        break;

    case eSANDTOOL_LIGHT:
        {
            m_sandBox->setLight( (fixedx - ((image.width()<<13)>>SANDGRID_DIV_BITS))/8,
                                 (fixedy - ((image.height()<<13)>>SANDGRID_DIV_BITS))/8);

        }

        break;

    }

}

void Widget::newSandFrame()
{
    int id[2];

    for (int ti=0; ti<3; ti++)
        if (m_paintingTool[ti]!=eSANDTOOL_NONE) {
            m_inactivityCounter = 0;
            m_toolBarVisibilityCounter = 7;

            id[0] = (m_paintingPosX[ti]-m_expaintingPosX[ti]);
            id[1] = (m_paintingPosY[ti]-m_expaintingPosY[ti]);

            int l = tmath_sqrt( (id[0]>>14)*(id[0]>>14) + (id[1]>>14)*(id[1]>>14));
            int divi = (m_paintingSize/2);
            if (divi<1) divi=1;
            int steps = l/divi + 1;            // lengh / 2 steps.
            if (m_paintingTool[ti]==eSANDTOOL_LIGHT) steps = 1;

            for (int s=0; s<steps; s++) {

                toolStep( m_expaintingPosX[ti] + id[0]*s/steps, m_expaintingPosY[ti] + id[1]*s/steps, ti, id );
            };

            // udpate paintingpos
            m_expaintingPosX[ti] = m_paintingPosX[ti];
            m_expaintingPosY[ti] = m_paintingPosY[ti];
        }
    m_tangle+=0.5f;
    m_sandBox->run( 65536>>6 );         // 64fps
}

void Widget::timerEvent(QTimerEvent *e)
{
    if (e->timerId()==m_fastUpdateTimer) {
        if (m_toolBarVisibilityCounter>0 && toolbar->isVisible()) toolbar->hide();

        m_callResetInactivity++;
        if (m_callResetInactivity>250) {

#ifdef Q_OS_SYMBIAN
            User::ResetInactivityTime();
#endif
            m_callResetInactivity = 0;
        }

        CTSurface_RGBA8888 renderRef;
        renderRef.setAsReference( image.bits(), image.width(), image.height(), image.bytesPerLine()/4, eTSURFACEFORMAT_RGBA8888 );

        // run intro instead of anything else
        if (m_intro) {
            m_toolBarVisibilityCounter = 4;
            int awidth = 320;
            int aheight = 200;
             CTSurface_RGBA8888 sub;
            sub.setReference( &renderRef, renderRef.getWidth()/2-awidth, renderRef.getHeight()/2-aheight, awidth*2, aheight*2 );

            if (m_intro->run(1024) == 0) {
                // Animate toolbar to visible
                QTimer::singleShot(2000, this, SLOT(startToolbarAnimation()));
                delete m_intro;
                m_intro = 0;

                if (m_inactivityCounter>0) {
                    m_sandBox->markAllChanged();
                    m_sandBox->render( renderRef );
                    update();            // just to prevent the full-update after first intro
                }

            } else {
                m_intro->draw(&sub);
                update(renderRef.getWidth()/2-awidth, renderRef.getHeight()/2-aheight, awidth*2, aheight*2 );
            }
            return;
        } else {
            m_inactivityCounter++;
            if (m_inactivityCounter>3000) {
                m_inactivityCounter = 1;
                if (1) {
                    m_intro = new CVSand_Intro(true);
                    update();
                }
            }
        }


        newSandFrame();
        int bounds[4];
        // draw the sandbox into m_image
        if (m_sandBox->render( renderRef, bounds ) != 0) {
            update(QRect(bounds[0], bounds[2], (bounds[1]-bounds[0])+SANDGRID_DIV, (bounds[3]-bounds[2])+SANDGRID_DIV));
        }

    }
#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN  ||defined Q_OS_ANDROID
    else if (e->timerId()==m_sensor_tid) {
        readSensor();
    }
#endif
    else if (e->timerId()==m_toolbar_tid) {
        if (m_toolBarVisibilityCounter>0)
            m_toolBarVisibilityCounter--;
        else {
            m_toolBarVisibilityCounter=0;
            if (toolbar->isVisible()==false) toolbar->show();
        }

    }
}


#define SHAKE_LIMIT 310000
void Widget::readSensor()
{
#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN ||defined Q_OS_ANDROID

    // Read rotation data
    QAccelerometerReading *reading = m_sensor->reading();
    // up/down rotation
    qreal x = reading->property("x").value<qreal>();
    qreal y = reading->property("y").value<qreal>();
    qreal z = reading->property("z").value<qreal>();


    int vx = (int)( x * (float)FP_VAL);
    int vy = (int)( y * (float)FP_VAL);
    int vz = (int)( z * (float)FP_VAL);


    int dif_amount = tmath_fpmul( vx, vx ) +
                     tmath_fpmul( vy, vy ) +
                     tmath_fpmul( vz, vz );

    dif_amount = tmath_fpsqrt( dif_amount );            // MIGHT not be needed?

    if (dif_amount> SHAKE_LIMIT ) {
        dif_amount -= SHAKE_LIMIT;
        m_sandBox->shake( vx,vy );
    };

#endif
}

bool Widget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        if (m_intro) m_intro->cancel();
        m_touchEnabled = true;
        QRect toolbarRect = QRect(toolbar->pos(),toolbar->size());
        toolbarRect.adjust(-20,-20,20,20);

        QList<QTouchEvent::TouchPoint> touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();
        foreach (const QTouchEvent::TouchPoint &touchPoint, touchPoints) {
            int ind = touchPoint.id();
            switch (touchPoint.state()) {
            case Qt::TouchPointPressed:
                {
                    // The touch point pressed
                    QPoint p = touchPoint.pos().toPoint();

                    // NOTE: Ignore touch if parent toolbar widget pressed
                    // Then system send mouse press to toolbar widget
                    if (toolbarRect.contains(p.x(),p.y()) && toolbar->isVisible()) {
                        event->ignore();
                        return false;
                    }


                    int x = (p.x()<<FP_BITS) >> SANDGRID_DIV_BITS;
                    int y = (p.y()<<FP_BITS) >> SANDGRID_DIV_BITS;
                    if (x>=0 && y>=0 && x<(m_sandBox->getGrid()->width() << FP_BITS) && y<(m_sandBox->getGrid()->height() << FP_BITS)) {
                        if (m_sandBox->isScratching()==false)
                            m_paintingTool[ind]= m_selectedTool;
                        else
                            m_paintingTool[ind] = eSANDTOOL_SCRATCH_TO_TARGET;

                        m_paintingPosX[ind] = x;
                        m_paintingPosY[ind] = y;
                        m_expaintingPosX[ind] = m_paintingPosX[ind];
                        m_expaintingPosY[ind] = m_paintingPosY[ind];
                    }
                    continue;
                }
            case Qt::TouchPointMoved:
                {
                    // The touch point moved
                    QPoint p = touchPoint.pos().toPoint();
                    if (m_paintingTool[ind] != eSANDTOOL_NONE) {
                        m_paintingPosX[ind] = (p.x()<<FP_BITS) >> SANDGRID_DIV_BITS;
                        m_paintingPosY[ind] = (p.y()<<FP_BITS) >> SANDGRID_DIV_BITS;
                    }
                    continue;
                }
            case Qt::TouchPointReleased:
                {
                    // The touch point was released
                    QPoint p = touchPoint.pos().toPoint();
                    m_paintingTool[ind] = eSANDTOOL_NONE;
                    continue;
                }
            case Qt::TouchPointStationary:
                {
                    // The touch point did NOT move
                    continue;
                }
            default:
                {
                    continue;
                }
            }
        }
        break;
    }
    default:
        return QWidget::event(event);
    }
    return true;
}


void Widget::mousePressEvent(QMouseEvent *e)
{
    if (m_intro) m_intro->cancel();
    QWidget::mousePressEvent(e);
    m_inactivityCounter = 1;

    if (!m_touchEnabled) {
        int x = (e->pos().x()<<FP_BITS) >> SANDGRID_DIV_BITS;
        int y = (e->pos().y()<<FP_BITS) >> SANDGRID_DIV_BITS;
        if (x>=0 && y>=0 && x<(m_sandBox->getGrid()->width() << FP_BITS) && y<(m_sandBox->getGrid()->height() << FP_BITS)) {
            if (m_sandBox->isScratching()==false){
               if (e->button() != Qt::RightButton)
                   m_paintingTool[0]= m_selectedTool;
               else
                   m_paintingTool[0] = eSANDTOOL_ADD;
            } else
                m_paintingTool[0] = eSANDTOOL_SCRATCH_TO_TARGET;

            m_paintingPosX[0] = x;
            m_paintingPosY[0] = y;
            m_expaintingPosX[0] = m_paintingPosX[0];
            m_expaintingPosY[0] = m_paintingPosY[0];
        }
    }
}


void Widget::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
    if (!m_touchEnabled) {
        if (m_paintingTool[0] != eSANDTOOL_NONE) {
            m_paintingPosX[0] = (e->pos().x()<<FP_BITS) >> SANDGRID_DIV_BITS;
            m_paintingPosY[0] = (e->pos().y()<<FP_BITS) >> SANDGRID_DIV_BITS;
        }
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    if (!m_touchEnabled) {
        m_paintingTool[0] = eSANDTOOL_NONE;
    }
}

void Widget::pressed(ToolBarButton::ButtonType type)
{
    // Toolbar button pressed

    switch (type) {
    case ToolBarButton::ENew:
    {
        m_sandBox->autoPaintToResetTarget( rand()&7 );
        break;
    }
    case ToolBarButton::ECamera:
    {
        startCamera();
        break;
    }
    case ToolBarButton::EOpen:
    {
#ifdef Q_WS_MAEMO_5
        QString filename = QFileDialog::getOpenFileName(this, tr("Open picture"),
                                                         "/home/user/MyDocs/DCIM",
                                                         tr("Images (*.png *.jpg *.jpeg)"));
#else
        QString filename = QFileDialog::getOpenFileName(this, tr("Open picture"),
                                                         QDir::home().path(),
                                                         tr("Images (*.png *.jpg *.jpeg)"));
#endif
        if (!filename.isEmpty()) {
            // Load image by QImageReader
            QImageReader* reader = new QImageReader();
            reader->setFileName(filename);

            // Scale size to fit into screen
            QSize imageSize = reader->size();
            QSize screenSize = size();
            imageSize.scale(screenSize, Qt::KeepAspectRatio);
            reader->setScaledSize(imageSize);

            // Reads image
            QImage loadedImage = reader->read();
            delete reader;

            // Sand image
            if (!loadedImage.isNull()) {
                m_inactivityCounter = 1;
                m_sandBox->markAllChanged();
                autopaint(loadedImage);
                update();
            }

        }
        break;
    }
    case ToolBarButton::EExit:
    {
        QApplication::exit(0);
        break;
    }
    case ToolBarButton::ESize:
    {
        m_paintingSize = toolbar->penSize();
        break;
    }
    case ToolBarButton::EMinimize:
    {
#if defined Q_WS_MAEMO_5
        QDBusConnection c = QDBusConnection::sessionBus();
        QDBusMessage m = QDBusMessage::createSignal("/","com.nokia.hildon_desktop","exit_app_view");
        c.send(m);
#endif
        break;
    }
    case ToolBarButton::EMMS:
    {
#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN

        // Stop UI update timer
        this->killTimer(m_fastUpdateTimer);
        m_fastUpdateTimer = 0;

        // Show contact to the user
        m_contactsDlg = new ContactsDialog(m_contactManager, this);
#if defined Q_WS_MAEMO_5
        m_contactsDlg->setMinimumSize(size().width(),404);
#elif defined Q_OS_SYMBIAN
        m_contactsDlg->showFullScreen();
#endif

        QObject::connect(m_contactsDlg, SIGNAL(contactSelected(QString)), this,
            SLOT(sendMMS(QString)));
        m_contactsDlg->exec();
        QObject::disconnect(m_contactsDlg, SIGNAL(contactSelected(QString)), this,
            SLOT(sendMMS(QString)));
        delete m_contactsDlg;

        // Start UI update timer
        m_fastUpdateTimer = this->startTimer(16);
#endif

        break;
    }
    case ToolBarButton::ELight:
    {
        m_selectedTool = eSANDTOOL_LIGHT;
        break;
    }
    case ToolBarButton::EEraser:
    {
        m_selectedTool = eSANDTOOL_SCRATCH_TO_TARGET;
        break;
    }
    case ToolBarButton::EAddSand:
    {
        m_selectedTool = eSANDTOOL_ADD;
        break;
    }
    case ToolBarButton::ERemoveSand:
    {
        m_selectedTool = eSANDTOOL_CURVE;
        break;
    }
    case ToolBarButton::ESand:
    {
        break;
    }
    case ToolBarButton::EPushSand:
    {
        m_selectedTool= eSANDTOOL_PUSH;
        break;
    }
    case ToolBarButton::ESnow:
    {
        m_selectedTool = eSANDTOOL_PUSH_TOWARDS;
        break;
    }
    case ToolBarButton::EHurricane:
    {
        m_selectedTool = eSANDTOOL_PULL;
        break;
    }
    default:
    {
        break;
    }
    };

    update();
}

void Widget::sendMMS(QString phoneNumber)
{
#if defined Q_WS_MAEMO_5 || defined Q_OS_SYMBIAN
    QMessage message;

    message.setType(QMessage::Mms);
    message.setParentAccountId(QMessageAccount::defaultAccount(QMessage::Mms));
    message.setTo(QMessageAddress(QMessageAddress::Phone, phoneNumber));

    // Save image to file first
    QString path = QDir::home().path();
    path.append("/QSandPaintMMS.png");

    // Remove previous file
    QFile file(path);
    if (file.exists())
        file.remove();
    file.close();

    // Save image smaller to fit into MMS message
    QSize mmsImageSize = QSize(size().width()*0.7,size().height()*0.7);
    QImage mmsImage = image.scaled(mmsImageSize,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    mmsImage.save(path);

    // Set saved file as attachment into MMS
    QStringList paths;
    paths << path;
    message.appendAttachments(paths);

    // Send MMS
    m_messageService->send(message);
#endif
}

void Widget::paintToPixmap()
{
    newSandFrame();
    update();
}

void Widget::startToolbarAnimation()
{
    if (timeLine) 
        return;
    
    timeLine = new QTimeLine(2000,this);
    timeLine->setCurveShape(QTimeLine::EaseInOutCurve);
    QObject::connect(timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(animate(qreal)));
    timeLine->start();
}

void Widget::animate(qreal value)
{
#if defined Q_WS_MAEMO_5
    // from bottom to top
    int endYPos = size().height() - 10;
    toolbar->move(toolbar->pos().x(), size().height() - (endYPos * value));

#else
    // from top to bottom
    int endYPos = size().height() - toolbar->height() - 10;
    toolbar->move(toolbar->pos().x(), endYPos * value);
#endif
}

void Widget::resizeEvent(QResizeEvent *event)
{
    // Set QPixmap (paper) size and update toolbar size/position
    QWidget::resizeEvent(event);
    toolbar->setMainWidgetSize(size());
    bool hide = true;
    if (timeLine) // Hide toolbar only before QTimeLine animation
        hide = false;
    
    #if defined Q_WS_MAEMO_5
    toolbar->updateToolbarPos(size(), 1, hide);
    #else
    toolbar->updateToolbarPos(size(), 2, hide);
    #endif
}


void Widget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawImage(e->rect(), image, e->rect() );

    if (m_intro)
        return;

    QString selectedTool;
    if (toolbar && toolbar->isVisible()) {
        switch (toolbar->downButton()) {
        case ToolBarButton::ELight:
            {
                selectedTool.append("Set light direction");
                break;
            }
        case ToolBarButton::EAddSand:
            {
                selectedTool.append("Add more sand");
                break;
            }
        case ToolBarButton::ERemoveSand:
            {
                selectedTool.append("Remove Sand");
                break;
            }
        case ToolBarButton::EPushSand:
            {
                selectedTool.append("Finger push");
                break;
            }
        case ToolBarButton::ESnow:
            {
                selectedTool.append("Shovel push");
                break;
            }
        case ToolBarButton::EHurricane:
            {
                selectedTool.append("Hurricane effect");
                break;
            }
        case ToolBarButton::EEraser:
            {
                selectedTool.append("Restore original image");
                break;
            }
        default:
            {
                break;
            }
        };

#if defined Q_WS_MAEMO_5
        painter.setFont(m_font);
        painter.setPen(Qt::black);
        QSize s = size();
        painter.drawText(QPoint(14,s.height()-19), selectedTool );
        painter.setPen(Qt::white);
        painter.drawText(QPoint(13,s.height()-20), selectedTool );
#else
        painter.setFont(m_font);
        painter.setPen(Qt::black);
        painter.drawText(QPoint(14,18+20), selectedTool );
        painter.setPen(Qt::white);
        painter.drawText(QPoint(13,17+20), selectedTool );
#endif
    }

#ifdef SANDGRID_DEBUG_ON
    QString s = QString("Amount: %1").arg(m_sandBox->getGrid()->m_totalAmount);
    painter.drawText(QPoint(10,50), s );

    s = QString("Bleeded: %1").arg(m_sandBox->getGrid()->m_bleededAmount);
    painter.drawText(QPoint(10,80), s );
#endif
}

void Widget::startCamera()
{
    QProcess::startDetached("CameraApp.exe");
    listenCamera();
}

void Widget::listenCamera()
{
#ifdef Q_OS_SYMBIAN
    m_cameraFileObserver->Start();
#endif
}

#ifdef Q_OS_SYMBIAN
void Widget::NewCameraFileL(const TFileName& aFileName)
{
    QString string((QChar*)aFileName.Ptr(),aFileName.Length());
    m_pathToImage = string;
    QTimer::singleShot(1000,this,SLOT(readCameraImage()));

    m_sandBox->markAllChanged();
    update();
}
#endif


void Widget::readCameraImage()
{
    QImageReader* reader = new QImageReader();
    reader->setFileName(m_pathToImage);
    if (!reader->canRead()) {
        m_inactivityCounter = 1;
        m_sandBox->markAllChanged();
        update();
        return;
    }

    QSize imageSize = reader->size();
    imageSize.scale(size(), Qt::KeepAspectRatio);
    reader->setScaledSize(imageSize);
    QImage image = reader->read();

    delete reader;

    if (!image.isNull()) {
        m_inactivityCounter = 1;
        m_sandBox->markAllChanged();
        autopaint(image);

        update();
    }

    // Move this application on top of camera app
    m_mainWindow->raise();
}

