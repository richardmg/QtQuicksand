/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef ToolBarButton_h
#define ToolBarButton_h

#include <QtWidgets>

class ToolBarSizeButton;
class ToolBarWidget;

/*****************************************************************************
 * ToolBarButton
 */
class ToolBarButton: public QWidget
{
Q_OBJECT

public:
    enum ButtonType
    {
        ENoType,
        EMinimize,
        ENew,
        ECamera,
        EOpen,
        EMMS,
        ESize,
        ELight,
        ESand,
        EAddSand,
        ERemoveSand,
        EPushSand,
        ESnow,
        EHurricane,
        EExit,
        EEraser
    };

    //EAddSand,
    //ECurveSand,

public:
    ToolBarButton(ToolBarButton::ButtonType type = ToolBarButton::ENoType, QWidget *parent = 0);
    virtual ~ToolBarButton();

    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    virtual void resizeEvent(QResizeEvent *);

    virtual void paintEvent(QPaintEvent *);
    virtual bool isPressed();
    virtual void setPressed(bool);

    virtual ToolBarButton::ButtonType btnType();

    virtual bool isToggleButton(ToolBarButton::ButtonType = ENoType);
    virtual bool isDialogButton();

public slots:
    virtual void someButtonPressed(ToolBarButton::ButtonType);

signals:
    void buttonPressed(ToolBarButton::ButtonType);

protected:
    ButtonType type;
    QPixmap upPixmap;
    QPixmap downPixmap;
    bool pressed;
    ToolBarWidget* toolbarwidget;
};

/*****************************************************************************
 * ToolBarSizeButton
 */
const int MIN_PEN_SIZE = 2;
const int PEN_APPEND_SIZE = 2;
const int MAX_PEN_SIZE = 6;
class ToolBarSizeButton: public ToolBarButton
{
Q_OBJECT

public:
    ToolBarSizeButton(QWidget *parent = 0);
    ~ToolBarSizeButton();

    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *);

    int penSize();
    void setNextPenSize();

private:
    void loadSvg();

private:
    int pSize;
};


/*****************************************************************************
 * ToolBarSandButton
 */
class ToolBarSandButton: public ToolBarButton
{
Q_OBJECT

public:
    enum SandButtonType
    {
        ERemoveSand = 0,
        EAddSand
    };

public:
    ToolBarSandButton(QWidget *parent = 0);
    ~ToolBarSandButton();

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *);

    void setNextPen();

private:
    void loadSvg();

private:
    int m_type;
};


#endif // ToolBarButton_h
