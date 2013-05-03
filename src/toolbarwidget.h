/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef ToolBarWidget_h
#define ToolBarWidget_h

#include <QtGui>
#include "toolbarbutton.h"

// Toolbar size consts
const int LandscapeToolbarHeight = 70;
const int LandscapeToolbarButtonHeight = 50;
const int PortraitToolbarWidth = 130;
const int PortraitToolbarButtonHeight = 50;

#if defined Q_WS_MAEMO_5
const int ButtonCount = 12;
#else
const int ButtonCount = 11;
#endif

const int LandscapeToolbarWidth = LandscapeToolbarButtonHeight * (ButtonCount+1);
const int PortraitToolbarHeight = (PortraitToolbarButtonHeight * (ButtonCount+1)) * 0.55;


class ToolBarWidget: public QWidget
{
Q_OBJECT

public:
    ToolBarWidget(QWidget *parent = 0);
    ~ToolBarWidget();

    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *);


    void createLandscapeLayout();
    void createPortraitLayout();
    void updateToolbarPos(QSize s, int posIndex, bool hide = false);

    void setDownButton(ToolBarButton::ButtonType);
    ToolBarButton::ButtonType downButton();

    int penSize();
    void setMainWidgetSize(QSize mainWidgetSize);

public slots:
    void buttonPressed(ToolBarButton::ButtonType);

signals:
    void pressed(ToolBarButton::ButtonType);

private:
    bool allowDocking;
public:
    bool dockingDone;

private:
    QPoint mousePress;
    int xOffset;
    int yOffset;

    QPointer<QHBoxLayout> landscapeLayout;
    QPointer<QVBoxLayout> portraitLayout;

    ToolBarButton* btnSwitch;
    ToolBarButton* btnNew;
    ToolBarButton* btnCamera;
    ToolBarButton* btnOpen;
    ToolBarButton* btnMMS;
    ToolBarSizeButton* btnSize;
    ToolBarButton* btnLight;
    ToolBarSandButton* btnSand;
    ToolBarButton* btnEraser;
    ToolBarButton* btnPush;
    ToolBarButton* btnSnow;
    ToolBarButton* btnHurricane;
    ToolBarButton* btnExit;

    ToolBarButton::ButtonType pressedButton;

    QPixmap landscapeBackground;
    QPixmap portraitBackground;

    QSize mainWidgetSize;
    int acceptMouseMoveTimer;
};

#endif // ToolBarWidget_h
