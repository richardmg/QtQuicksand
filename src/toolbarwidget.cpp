/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "toolbarwidget.h"
#include "toolbarbutton.h"
#include <QDebug>

ToolBarWidget::ToolBarWidget(QWidget *parent) :
    QWidget(parent)
{
    // portraitBackground = 0;
    // landscapeBackground = 0;

    // Create buttons

#if defined Q_WS_MAEMO_5
    btnSwitch = new ToolBarButton(ToolBarButton::EMinimize, this);
#endif
    btnNew = new ToolBarButton(ToolBarButton::ENew, this);
#if defined Q_OS_SYMBIAN
    btnCamera = new ToolBarButton(ToolBarButton::ECamera, this);
#else
    btnOpen = new ToolBarButton(ToolBarButton::EOpen, this);
#endif
    btnMMS = new ToolBarButton(ToolBarButton::EMMS, this);
    btnSize = new ToolBarSizeButton(this);
    btnLight = new ToolBarButton(ToolBarButton::ELight, this);
    btnSand = new ToolBarSandButton(this);
    btnEraser = new ToolBarButton(ToolBarButton::EEraser,this);
    btnPush = new ToolBarButton(ToolBarButton::EPushSand, this);
    btnSnow = new ToolBarButton(ToolBarButton::ESnow, this);
    btnHurricane = new ToolBarButton(ToolBarButton::EHurricane, this);
    btnExit = new ToolBarButton(ToolBarButton::EExit, this);


    // Listen general button pressed
#if defined Q_WS_MAEMO_5
    QObject::connect(btnSwitch, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
#endif
    QObject::connect(btnNew, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
#if defined Q_OS_SYMBIAN
    QObject::connect(btnCamera, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
#else
    QObject::connect(btnOpen, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
#endif
    QObject::connect(btnMMS, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnSize, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnLight, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnEraser, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnPush, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnSnow, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnHurricane, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnExit, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));
    QObject::connect(btnSand, SIGNAL(buttonPressed(ToolBarButton::ButtonType)),this, SLOT(buttonPressed(ToolBarButton::ButtonType)));

    // Togglebuttons
    // Listen button pressed signal for these that have to rise up when other are down
    QObject::connect(this, SIGNAL(pressed(ToolBarButton::ButtonType)),btnLight, SLOT(someButtonPressed(ToolBarButton::ButtonType)));
    QObject::connect(this, SIGNAL(pressed(ToolBarButton::ButtonType)),btnPush, SLOT(someButtonPressed(ToolBarButton::ButtonType)));
    QObject::connect(this, SIGNAL(pressed(ToolBarButton::ButtonType)),btnSnow, SLOT(someButtonPressed(ToolBarButton::ButtonType)));
    QObject::connect(this, SIGNAL(pressed(ToolBarButton::ButtonType)),btnHurricane, SLOT(someButtonPressed(ToolBarButton::ButtonType)));
    QObject::connect(this, SIGNAL(pressed(ToolBarButton::ButtonType)),btnEraser, SLOT(someButtonPressed(ToolBarButton::ButtonType)));
    QObject::connect(this, SIGNAL(pressed(ToolBarButton::ButtonType)),btnSand, SLOT(someButtonPressed(ToolBarButton::ButtonType)));

    allowDocking = false;
}


void ToolBarWidget::setDownButton(ToolBarButton::ButtonType type)
{
    // Pressed down (all the time) only for these buttons
    switch (type) {
    case ToolBarButton::ELight:
    {
        btnLight->setPressed(true);
        pressedButton = type;
        break;
    }
    case ToolBarButton::ESand:
    case ToolBarButton::EAddSand:
    case ToolBarButton::ERemoveSand:
    {
        btnSand->setPressed(true);
        pressedButton = type;
        break;
    }
    case ToolBarButton::EPushSand:
    {
        btnPush->setPressed(true);
        pressedButton = type;
        break;
    }
    case ToolBarButton::ESnow:
    {
        btnSnow->setPressed(true);
        pressedButton = type;
        break;
    }
    case ToolBarButton::EHurricane:
    {
        btnHurricane->setPressed(true);
        pressedButton = type;
        break;
    }
    case ToolBarButton::EEraser:
    {
        btnEraser->setPressed(true);
        pressedButton = type;
        break;
    }
    default:
    {
        break;
    }
    };
}

ToolBarButton::ButtonType ToolBarWidget::downButton()
{
    return pressedButton;
}

int ToolBarWidget::penSize()
{
    return btnSize->penSize();
}

void ToolBarWidget::updateToolbarPos(QSize s, int posIndex, bool hide)
{
    /* posIndex:
     * 1 = bottom
     * 2 = top
     * 3 = left
     * 4 = right
     */

    if (posIndex == 1) {
        // to BOTTOM
        createLandscapeLayout();
        if (hide) {
            setGeometry((s.width() - LandscapeToolbarWidth) / 2, s.height(), LandscapeToolbarWidth,
                LandscapeToolbarHeight);
        }
        else {
            setGeometry((s.width() - LandscapeToolbarWidth) / 2, s.height()
                - LandscapeToolbarHeight - 10, LandscapeToolbarWidth, LandscapeToolbarHeight);
        }
    }
    else if (posIndex == 2) {
        // to TOP
        createLandscapeLayout();
        if (hide) {
            setGeometry((s.width() - LandscapeToolbarWidth) / 2, LandscapeToolbarHeight * -1,
                LandscapeToolbarWidth, LandscapeToolbarHeight);
        }
        else {
            setGeometry((s.width() - LandscapeToolbarWidth) / 2, 10, LandscapeToolbarWidth,
                LandscapeToolbarHeight);
        }
    }
    else if (posIndex == 3) {
        // to LEFT
        createPortraitLayout();
        setGeometry(10, (s.height() - PortraitToolbarHeight) / 2, PortraitToolbarWidth,
            PortraitToolbarHeight);
    }
    else if (posIndex == 4) {
        // to RIGHT
        createPortraitLayout();
        setGeometry(s.width() - PortraitToolbarWidth - 10,
            (s.height() - PortraitToolbarHeight) / 2, PortraitToolbarWidth, PortraitToolbarHeight);
    }
}

void ToolBarWidget::createPortraitLayout()
{
    // Clean previous layout
    if (landscapeLayout) {
        for (int i = 0; i < landscapeLayout->count(); i++) {
            landscapeLayout->removeItem(landscapeLayout->itemAt(i));
        }
        delete landscapeLayout;
        landscapeLayout = 0;
    }

    // Create new one
    if (!portraitLayout) {
        portraitLayout = new QVBoxLayout(this);
        portraitLayout->setSpacing(2);
        // int left, int top, int right, int bottom
        portraitLayout->setContentsMargins(2, 6, 2, 6);

#if defined Q_WS_MAEMO_5 || defined Q_OS_ANDROID
        QHBoxLayout* l = new QHBoxLayout();
        l->addWidget(btnSwitch);
        l->setAlignment(btnSwitch, Qt::AlignCenter);
        l->addWidget(btnNew);
        l->setAlignment(btnNew, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnOpen);
        l->setAlignment(btnOpen, Qt::AlignCenter);
        l->addWidget(btnMMS);
        l->setAlignment(btnMMS, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnSize);
        l->setAlignment(btnSize, Qt::AlignCenter);
        l->addWidget(btnLight);
        l->setAlignment(btnLight, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnSand);
        l->setAlignment(btnSand, Qt::AlignCenter);
        l->addWidget(btnEraser);
        l->setAlignment(btnEraser, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnPush);
        l->setAlignment(btnPush, Qt::AlignCenter);
        l->addWidget(btnSnow);
        l->setAlignment(btnSnow, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnHurricane);
        l->setAlignment(btnHurricane, Qt::AlignCenter);
        l->addWidget(btnExit);
        l->setAlignment(btnExit, Qt::AlignCenter);
        portraitLayout->addLayout(l);
#else
        QHBoxLayout* l = new QHBoxLayout();
        l->addWidget(btnNew);
        l->setAlignment(btnNew, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnCamera);
        l->setAlignment(btnCamera, Qt::AlignCenter);
        l->addWidget(btnMMS);
        l->setAlignment(btnMMS, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnSize);
        l->setAlignment(btnSize, Qt::AlignCenter);
        l->addWidget(btnLight);
        l->setAlignment(btnLight, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnSand);
        l->setAlignment(btnSand, Qt::AlignCenter);
        l->addWidget(btnEraser);
        l->setAlignment(btnEraser, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnPush);
        l->setAlignment(btnPush, Qt::AlignCenter);
        l->addWidget(btnSnow);
        l->setAlignment(btnSnow, Qt::AlignCenter);
        portraitLayout->addLayout(l);

        l = new QHBoxLayout();
        l->addWidget(btnHurricane);
        l->setAlignment(btnHurricane, Qt::AlignCenter);
        l->addWidget(btnExit);
        l->setAlignment(btnExit, Qt::AlignCenter);
        portraitLayout->addLayout(l);
#endif
        
        
        setLayout(portraitLayout);
    }
    if (portraitBackground.isNull()) {
        //landscapeBackground = 0;
        portraitBackground.load(":/gfx/graf_bar_portrait_fat.svg");
    }
}

void ToolBarWidget::createLandscapeLayout()
{
    // Clean previous layout
    if (portraitLayout) {
        for (int i = 0; i < portraitLayout->count(); i++) {
            portraitLayout->removeItem(portraitLayout->itemAt(i));
        }
        delete portraitLayout;
        portraitLayout = 0;
    }

    // Create new one
    if (!landscapeLayout) {
        landscapeLayout = new QHBoxLayout(this);
        landscapeLayout->setSpacing(2);
        // int left, int top, int right, int bottom
        landscapeLayout->setContentsMargins(2, 6, 2, 6);

#if defined Q_WS_MAEMO_5
        landscapeLayout->addWidget(btnSwitch);
        landscapeLayout->setAlignment(btnSwitch, Qt::AlignCenter);
#endif   
        landscapeLayout->addWidget(btnNew);
        landscapeLayout->setAlignment(btnNew, Qt::AlignCenter);
#if defined Q_OS_SYMBIAN
        landscapeLayout->addWidget(btnCamera);
        landscapeLayout->setAlignment(btnCamera, Qt::AlignCenter);
#else
        landscapeLayout->addWidget(btnOpen);
        landscapeLayout->setAlignment(btnOpen, Qt::AlignCenter);
#endif
        landscapeLayout->addWidget(btnMMS);
        landscapeLayout->setAlignment(btnMMS, Qt::AlignCenter);
        landscapeLayout->addWidget(btnSize);
        landscapeLayout->setAlignment(btnSize, Qt::AlignCenter);
        landscapeLayout->addWidget(btnLight);
        landscapeLayout->setAlignment(btnLight, Qt::AlignCenter);
        landscapeLayout->addWidget(btnSand);
        landscapeLayout->setAlignment(btnSand, Qt::AlignCenter);
        landscapeLayout->addWidget(btnEraser);
        landscapeLayout->setAlignment(btnEraser, Qt::AlignCenter);
        landscapeLayout->addWidget(btnPush);
        landscapeLayout->setAlignment(btnPush, Qt::AlignCenter);
        landscapeLayout->addWidget(btnSnow);
        landscapeLayout->setAlignment(btnSnow, Qt::AlignCenter);
        landscapeLayout->addWidget(btnHurricane);
        landscapeLayout->setAlignment(btnHurricane, Qt::AlignCenter);
        landscapeLayout->addWidget(btnExit);
        landscapeLayout->setAlignment(btnExit, Qt::AlignCenter);

        setLayout(landscapeLayout);
    }
    if (landscapeBackground.isNull()) {
        //portraitBackground = 0;
        landscapeBackground.load(":/gfx/graf_bar_wide_landscape.svg");
    }
}

void ToolBarWidget::buttonPressed(ToolBarButton::ButtonType type)
{
    setDownButton(type);
    emit pressed(type);
}

ToolBarWidget::~ToolBarWidget()
{
}

void ToolBarWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (!landscapeBackground.isNull()) {
        landscapeBackground = landscapeBackground.scaled(size(), Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation);

        // Set background as a transparent
        QPixmap transparent(landscapeBackground.size());
        transparent.fill(Qt::transparent);
        QPainter p(&transparent);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawPixmap(0, 0, landscapeBackground);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.fillRect(transparent.rect(), QColor(0, 0, 0, 150));
        p.end();
        landscapeBackground = transparent;
    }
    else if (!portraitBackground.isNull()) {
        portraitBackground = portraitBackground.scaled(size(), Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation);

        // Set background as a transparent
        QPixmap transparent(portraitBackground.size());
        transparent.fill(Qt::transparent);
        QPainter p(&transparent);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        p.drawPixmap(0, 0, portraitBackground);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.fillRect(transparent.rect(), QColor(0, 0, 0, 150));
        p.end();
        portraitBackground = transparent;
    }
}

void ToolBarWidget::timerEvent(QTimerEvent *event)
{
    if (acceptMouseMoveTimer == event->timerId()) {
        allowDocking = true;
        dockingDone = true;
        killTimer(acceptMouseMoveTimer);
        acceptMouseMoveTimer = -1;
        update();
    }
}


void ToolBarWidget::mousePressEvent(QMouseEvent *event)
{
    event->accept(); // NOTE: Event does not go to main widget to do painting
    mousePress = mapToParent(event->pos());
    xOffset = mousePress.x() - pos().x();
    yOffset = mousePress.y() - pos().y();

    dockingDone = false;
    allowDocking = false;
    // Start timer for accepting mouse move
    if (acceptMouseMoveTimer != -1) {
        killTimer(acceptMouseMoveTimer);
        acceptMouseMoveTimer = -1;
    }
    acceptMouseMoveTimer = startTimer(1000);

}

void ToolBarWidget::setMainWidgetSize(QSize mainWidgetSize)
{
    this->mainWidgetSize = mainWidgetSize;
}

void ToolBarWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->accept(); // NOTE: Event does not go to main widget to do painting

    if (allowDocking) {
        QPoint p = this->mapToParent(event->pos());
        p.setX(p.x() - xOffset);
        p.setY(p.y() - yOffset);
        setGeometry(QRect(p, size()));

        int vCap = 5;
        int hCap = -50;

        QRect r = geometry();
        if (r.topLeft().y() < vCap) {
            // to Top
            updateToolbarPos(mainWidgetSize, 2);
            allowDocking = false;
        }
        else if (r.bottomRight().y() > mainWidgetSize.height() - vCap) {
            // to Bottom
            updateToolbarPos(mainWidgetSize, 1);
            allowDocking = false;
        }
        else if (r.topLeft().x() < hCap) {
            // to Left
            updateToolbarPos(mainWidgetSize, 3);
            allowDocking = false;
        }
        else if (r.bottomRight().x() > mainWidgetSize.width() - hCap) {
            // to Right
            updateToolbarPos(mainWidgetSize, 4);
            allowDocking = false;
        }
    }
}

void ToolBarWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (acceptMouseMoveTimer != -1) {
        killTimer(acceptMouseMoveTimer);
        acceptMouseMoveTimer = -1;
    }
    event->accept(); // NOTE: Event does not go to main widget to do painting
    allowDocking = false;
    dockingDone = false;
    update();
}

void ToolBarWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QRect r;

    // Background for the toolbar
    if (!landscapeBackground.isNull()) {
        painter.drawPixmap(rect().topLeft(), landscapeBackground);
        r = QRect(rect().topLeft(),QSize(LandscapeToolbarWidth,LandscapeToolbarHeight));
    }
    else if (!portraitBackground.isNull()) {
        painter.drawPixmap(rect().topLeft(), portraitBackground);
        r = QRect(rect().topLeft(),QSize(PortraitToolbarWidth,PortraitToolbarHeight));
    }

    QColor c(0,102,204,50);
    QBrush b(c);
    // Draw dragging line
    if (allowDocking) {
        painter.setBrush(b);
        painter.setPen(c);
        painter.drawRoundRect(rect(),10,10);
    }
}


