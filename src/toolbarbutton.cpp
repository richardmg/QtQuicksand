/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "toolbarbutton.h"
#include "toolbarwidget.h"
#include <QDebug>

/*****************************************************************************
 * ToolBarButton
 */
ToolBarButton::ToolBarButton(ToolBarButton::ButtonType type, QWidget *p) :
    QWidget(p)
{
    this->type = type;
    this->toolbarwidget = qobject_cast<ToolBarWidget*>(p);
    pressed = false;
    //downPixmap = 0;
    //upPixmap = 0;
    setMinimumSize(LandscapeToolbarButtonHeight, LandscapeToolbarButtonHeight);
}

ToolBarButton::~ToolBarButton()
{
}

void ToolBarButton::someButtonPressed(ToolBarButton::ButtonType btnType)
{
    // Check togglebuttons
    if (isToggleButton(btnType)) {
        if (isToggleButton()) {
            if (btnType != type && pressed) {
                pressed = false;
                update();
            }
        }
    }
}

bool ToolBarButton::isToggleButton(ToolBarButton::ButtonType t)
{
    if (t == ToolBarButton::ENoType) {
        t = this->type;
    }

    // Togglebutton
    // Buttons that can be down and rest have to be up
    if (t == ToolBarButton::ELight || t == ToolBarButton::EPushSand || t == ToolBarButton::EEraser
        || t == ToolBarButton::ESnow || t == ToolBarButton::EHurricane || t == ToolBarButton::ESand ||
        t == ToolBarButton::EAddSand || t == ToolBarButton::ERemoveSand) {
        return true;
    }
    else {
        return false;
    }
}

bool ToolBarButton::isDialogButton()
{
    if (type == ToolBarButton::EOpen || type == ToolBarButton::ECamera || type
        == ToolBarButton::EMMS || type == ToolBarButton::EMinimize) {
        return true;
    }
    else {
        return false;
    }
}

bool ToolBarButton::isPressed()
{
    return pressed;
}

void ToolBarButton::setPressed(bool p)
{
    pressed = p;
}

void ToolBarButton::resizeEvent(QResizeEvent *event)
{
    if (downPixmap.isNull() && upPixmap.isNull()) {

        // Load right button svg image
        switch (type) {

        case ToolBarButton::EMinimize:
        {
            upPixmap.load(":/gfx/icon_switch_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_switch_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::ENew:
        {
            upPixmap.load(":/gfx/icon_new_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_new_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::ECamera:
        {
            upPixmap.load(":/gfx/icon_camera_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_camera_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::EOpen:
        {
            upPixmap.load(":/gfx/icon_open_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_open_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::EMMS:
        {
            upPixmap.load(":/gfx/icon_MMS_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_MMS_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::ELight:
        {
            upPixmap.load(":/gfx/icon_light_direction_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_light_direction_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::EPushSand:
        {
            upPixmap.load(":/gfx/icon_push_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_push_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::ESnow:
        {
            upPixmap.load(":/gfx/icon_lumikola_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_lumikola_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::EHurricane:
        {
            upPixmap.load(":/gfx/icon_tornado_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_tornado_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::EExit:
        {
            upPixmap.load(":/gfx/icon_exit_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_exit_pressed_38x38.svg");
            break;
        }
        case ToolBarButton::EEraser:
        {
            upPixmap.load(":/gfx/icon_eraser_unpressed_38x38.svg");
            downPixmap.load(":/gfx/icon_eraser_pressed_38x38.svg");
            break;
        }
        default:
        {
            return;
        }
        };

        // Scale it
        upPixmap = upPixmap.scaled(event->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        downPixmap
            = downPixmap.scaled(event->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void ToolBarButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (!downPixmap.isNull() && !upPixmap.isNull()) {
        if (pressed) {
            painter.drawPixmap(this->rect().topLeft(), downPixmap);
        }
        else {
            painter.drawPixmap(this->rect().topLeft(), upPixmap);
        }
    }
}

ToolBarButton::ButtonType ToolBarButton::btnType()
{
    return type;
}

void ToolBarButton::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    pressed = true;
    repaint();
    event->ignore(); // Move event goes to ToolBarWidget
    if (!isDialogButton() && type != ToolBarButton::ENew && type != ToolBarButton::EMMS) {
        // Send button pressed signal
        // For some button buttons signal is sended in mouseReleaseEvent
        emit buttonPressed(type);
    }
}

void ToolBarButton::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    event->ignore(); // Move event goes to ToolBarWidget
}

void ToolBarButton::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore(); // Move event goes to ToolBarWidget

    if (isToggleButton()) {
        // Leave tool buttons pressed
    }
    else {
        // Lift other buttons back to up
        pressed = false;
        repaint();

        if (toolbarwidget->dockingDone) {
            // User was only moving toolbar, do not open dialogs
        }
        else {
            // Open dialogs
            // Send button pressed signal for dialog and undo/new buttons
            if (isDialogButton() || type == ToolBarButton::ENew || type != ToolBarButton::EMMS) {
                emit buttonPressed(type);
            }
        }
    }

    QWidget::mouseReleaseEvent(event);
}





/*****************************************************************************
 * ToolBarSizeButton
 */
ToolBarSizeButton::ToolBarSizeButton(QWidget *parent) :
    ToolBarButton(ToolBarButton::ESize, parent)
{
    pSize = MAX_PEN_SIZE;
}

ToolBarSizeButton::~ToolBarSizeButton()
{
}

int ToolBarSizeButton::penSize()
{
    return this->pSize;
}

void ToolBarSizeButton::setNextPenSize()
{
    if (pSize < MAX_PEN_SIZE) {
        pSize = pSize + PEN_APPEND_SIZE;
    }
    else {
        pSize = MIN_PEN_SIZE;
    }
}

void ToolBarSizeButton::loadSvg()
{
    //upPixmap = 0;
    //downPixmap = 0;

    if (pSize == MIN_PEN_SIZE) {
        upPixmap.load(":/gfx/icon_line_up_1st_selected_38x38.svg");
        downPixmap.load(":/gfx/icon_line_down_1st_selected_38x38.svg");
    }
    else if (pSize == MIN_PEN_SIZE + PEN_APPEND_SIZE) {
        upPixmap.load(":/gfx/icon_line_up_2nd_selected_38x38.svg");
        downPixmap.load(":/gfx/icon_line_down_2nd_selected_38x38.svg");
    }
    else {
        upPixmap.load(":/gfx/icon_line_up_3rd_selected_38x38.svg");
        downPixmap.load(":/gfx/icon_line_down_3rd_selected_38x38.svg");
    }

    if (!downPixmap.isNull() && !upPixmap.isNull()) {
        upPixmap = upPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        downPixmap = downPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void ToolBarSizeButton::resizeEvent(QResizeEvent *)
{
    loadSvg();
}

void ToolBarSizeButton::mousePressEvent(QMouseEvent *event)
{
    setNextPenSize();
    loadSvg();

    // Call base class implementation
    ToolBarButton::mousePressEvent(event);
}



/*****************************************************************************
 * ToolBarSandButton
 */
ToolBarSandButton::ToolBarSandButton(QWidget *parent) :
        ToolBarButton(ToolBarButton::ESand,parent)
{
    pressed = false;
    m_type = ToolBarSandButton::EAddSand;
}

ToolBarSandButton::~ToolBarSandButton()
{
}

void ToolBarSandButton::setNextPen()
{
    m_type++;
    if (m_type>1)
        m_type = 0;

    if (m_type == ToolBarSandButton::EAddSand)
        this->type = ToolBarButton::EAddSand;
    else
        this->type = ToolBarButton::ERemoveSand;

    qDebug() << "setNextPen " << m_type;

}

void ToolBarSandButton::loadSvg()
{
    //upPixmap = 0;
    //downPixmap = 0;

    if (m_type == ToolBarSandButton::EAddSand) {
        // Add sand
        upPixmap.load(":/gfx/icon_addremove_unpressed_38x38.svg");
        downPixmap.load(":/gfx/icon_add2_pressed_38x38.svg");
    }
    else{
        // Remove sand
        upPixmap.load(":/gfx/icon_addremove_unpressed_38x38.svg");
        downPixmap.load(":/gfx/icon_remove2_pressed_38x38.svg");
    }

    if (!downPixmap.isNull() && !upPixmap.isNull()) {
        upPixmap = upPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        downPixmap = downPixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}

void ToolBarSandButton::resizeEvent(QResizeEvent *)
{
    loadSvg();
}

void ToolBarSandButton::mousePressEvent(QMouseEvent *event)
{
    if (pressed) {
        setNextPen();
        loadSvg();
        emit buttonPressed(type);
        pressed = true;
        event->ignore(); // Move event goes to ToolBarWidget
        repaint();
    } else {
        if (m_type == ToolBarSandButton::EAddSand) {
            this->type = ToolBarButton::EAddSand;
        } else {
            this->type = ToolBarButton::ERemoveSand;
        }
        emit buttonPressed(type);
        pressed = true;
        repaint();
        event->ignore(); // Move event goes to ToolBarWidget
        repaint();
    }

}

void ToolBarSandButton::mouseReleaseEvent(QMouseEvent *event)
{
    event->ignore(); // Move event goes to ToolBarWidget
}


