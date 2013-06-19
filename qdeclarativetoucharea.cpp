/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QML Touch Area plugin of the Qt Toolkit.
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


#include "qdeclarativetoucharea.h"
#include <QEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDeclarativeExpression>
#include <QDeclarativeInfo>
#include <math.h>

QDeclarativeTouchArea::QDeclarativeTouchArea(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      _minimumTouches(0),
      _maximumTouches(INT_MAX),
      _scaleFactor(1.0),
      _rotationAngle(0.0),
      _parentTouchArea(this),
      _active(false),
      _stealMouse(false)
{
    setAcceptTouchEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildEvents(true);
}

void QDeclarativeTouchArea::updateTopTouchArea() {
    for (QDeclarativeItem* pItem = parentItem(); pItem != 0; pItem = pItem->parentItem()) {
        if (QDeclarativeTouchArea* ta = qobject_cast<QDeclarativeTouchArea*>(pItem)) {
            _parentTouchArea = ta;
            break;
        }
    }
}

QDeclarativeTouchArea::~QDeclarativeTouchArea() {
}

bool QDeclarativeTouchArea::sceneEvent(QEvent *event) {
    bool rv = false;
    switch (event->type()) {
    case QEvent::TouchBegin:
        updateTopTouchArea();
        if (_parentTouchArea != this) {
            if (_active || !_parentTouchArea->_active) {
                _active = true;
                _parentTouchArea->_active = true;
                updateTouchData(event);
                rv = true;
            }
        } else {
            updateTouchData(event);
            rv = true;
        }
        break;
    case QEvent::TouchUpdate:
        updateTouchData(event);
        rv = true;
        break;
    case QEvent::TouchEnd: {
        updateTouchData(event);
        if (_parentTouchArea != this) {
            if (_touches.empty()) {
                _active = false;
                _parentTouchArea->_active = false;
            }
        }
        rv = true;
        break;
    }
    default:
        break;
    }

    if (!rv)
        rv = QDeclarativeItem::sceneEvent(event);
    if (event->type() == QEvent::UngrabMouse) {
        setKeepMouseGrab(false);
    }
    return rv;
}

void QDeclarativeTouchArea::updateTouchData(QEvent *event) {
    bool ended = false;
    bool moved = false;
    bool started = false;

    QDeclarativeItem *grabber = scene() ? qobject_cast<QDeclarativeItem*>(scene()->mouseGrabberItem()) : 0;
    QDeclarativeTouchArea *touchGrabber = scene() ? qobject_cast<QDeclarativeTouchArea*>(scene()->mouseGrabberItem()) : 0;
    if (grabber == this || !grabber || !grabber->keepMouseGrab() || touchGrabber) {
        if (_stealMouse && (grabber != this || !grabber->keepMouseGrab())) {
            grabMouse();
            setKeepMouseGrab(true);
        }

        if (!_keepMouseFocus.script().isEmpty()) {
            QDeclarativeExpression expr(_keepMouseFocus.context(), _keepMouseFocus.scopeObject(), _keepMouseFocus.script());
            QVariant result = expr.evaluate();
            if (expr.hasError())
                qmlInfo(this) << expr.error();
            else
                _stealMouse = true;
        }
    }

    QTouchEvent *e = static_cast<QTouchEvent*>(event);
    clearChangedAndReleasedTouches();

    QList<QTouchEvent::TouchPoint> touchPoints = e->touchPoints();
    int numTouchPoints = touchPoints.count();
    if (numTouchPoints >= _minimumTouches && numTouchPoints <= _maximumTouches) {
        foreach (QTouchEvent::TouchPoint p, touchPoints) {
            Qt::TouchPointState touchPointState = p.state();
            int id = p.id();
            if (touchPointState & Qt::TouchPointReleased) {
                QDeclarativeTouchPoint* dtp = static_cast<QDeclarativeTouchPoint*>(_touches[id]);
                _changedTouches.insert(id,dtp);
                _releasedTouches.append(dtp);
                _touches.remove(id);
                ended = true;
            } else if (touchPointState & Qt::TouchPointMoved) {
                updateTouchPoint(static_cast<QDeclarativeTouchPoint*>(_touches[id]),&p);
                updatePinch(&touchPoints);
                moved = true;
            } else if (!_touches.contains(id)) {
                    addTouchPoint(&p);
                    started = true;
            }
        }
        if (ended) emit(touchEnd());
        if (moved) emit(touchMove());
        if (started) emit(touchStart());
    }
    event->accept();
}

void QDeclarativeTouchArea::clearChangedAndReleasedTouches() {
    foreach (QObject *p, _releasedTouches) {
        QDeclarativeTouchPoint* dtp = static_cast<QDeclarativeTouchPoint*>(p);
        if (!dtp->isQmlReferenced())
            delete dtp;
        else
            dtp->setValid(false);
    }
    _changedTouches.clear();
    _releasedTouches.clear();
}

void QDeclarativeTouchArea::addTouchPoint(const QTouchEvent::TouchPoint *p) {
    QDeclarativeTouchPoint *dtp = 0;
    foreach (QObject* proto, _touchPrototypes) {
        QDeclarativeTouchPoint* tp = static_cast<QDeclarativeTouchPoint*>(proto);
        if (!tp->isValid()) {
            tp->setValid(true);
            dtp = tp;
            break;
        }
    }

    if (dtp == 0)
        dtp = new QDeclarativeTouchPoint(false);
    dtp->setId(p->id());
    updateTouchPoint(dtp,p);
    _touches.insert(p->id(),dtp);
}

void QDeclarativeTouchArea::addTouchPrototype(QDeclarativeTouchPoint *prototype) {
    int id = _touchPrototypes.count();
    prototype->setId(id);
    _touchPrototypes.insert(id, prototype);
}

void QDeclarativeTouchArea::updateTouchPoint(QDeclarativeTouchPoint *dtp, const QTouchEvent::TouchPoint *p) {
    dtp->setX(p->pos().x());
    dtp->setY(p->pos().y());
    dtp->setSceneX(p->scenePos().x());
    dtp->setSceneY(p->scenePos().y());
    _changedTouches.insert(dtp->id(),dtp);
}

void QDeclarativeTouchArea::updatePinch(QList<QTouchEvent::TouchPoint> *touchPoints) {
    if (touchPoints->count()==2) {
        QTouchEvent::TouchPoint tp1 = touchPoints->at(0);
        QTouchEvent::TouchPoint tp2 = touchPoints->at(1);

        QPointF tp1LastPos = tp1.lastPos();
        QPointF tp2LastPos = tp2.lastPos();
        QPointF tp1Pos = tp1.pos();
        QPointF tp2Pos = tp2.pos();

        QPointF deltaA = tp1LastPos - tp2LastPos;
        QPointF deltaB = tp1Pos - tp2Pos;

        qreal distanceA = sqrt(pow(deltaA.x(),2.0)+pow(deltaA.y(),2.0));
        qreal distanceB = sqrt(pow(deltaB.x(),2.0)+pow(deltaB.y(),2.0));

        if (distanceA != 0 && distanceB != 0) {
            _scaleFactor*=(distanceB/distanceA);
            emit(scaleFactorChanged());
        }

        QLineF lineA(tp1LastPos, tp2LastPos);
        QLineF lineB(tp1Pos,tp2Pos);
        _rotationAngle-=lineA.angleTo(lineB);
        emit(rotationAngleChanged());
    }

}

bool QDeclarativeTouchArea::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate: {
            updateTouchData(event);
        }
        return true;
    case QEvent::TouchEnd: {
            updateTouchData(event);
            _stealMouse = false;
            QDeclarativeTouchArea *touchGrabber = scene() ? qobject_cast<QDeclarativeTouchArea*>(scene()->mouseGrabberItem()) : 0;
            if (touchGrabber == this)
                ungrabMouse();
            setKeepMouseGrab(false);
        }
        return true;
    default:
        return QDeclarativeItem::event(event);
    }

    return QDeclarativeItem::event(event);
}

bool QDeclarativeTouchArea::sceneEventFilter(QGraphicsItem *i, QEvent *event)
{
    if (!isVisible())
        return QDeclarativeItem::sceneEventFilter(i, event);
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
            updateTouchData(event);
        return true;
    case QEvent::TouchEnd: {
            updateTouchData(event);
            _stealMouse = false;
            QDeclarativeTouchArea *touchGrabber = scene() ? qobject_cast<QDeclarativeTouchArea*>(scene()->mouseGrabberItem()) : 0;
            if (touchGrabber == this)
                ungrabMouse();
            setKeepMouseGrab(false);
        }
        return true;
    default:
        break;
    }
    return QDeclarativeItem::sceneEventFilter(i, event);
}

