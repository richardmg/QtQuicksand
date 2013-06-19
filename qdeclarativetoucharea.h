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


#ifndef QDECLARATIVETOUCHAREA_H
#define QDECLARATIVETOUCHAREA_H

#include "qdeclarativeitem.h"
#include "qevent.h"

#include <QMap>
#include <QList>
#include <QDeclarativeScriptString>

class QDeclarativeTouchPoint : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ id)
    Q_PROPERTY(qreal x READ x NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y NOTIFY yChanged)
    Q_PROPERTY(qreal sceneX READ sceneX NOTIFY sceneXChanged)
    Q_PROPERTY(qreal sceneY READ sceneY NOTIFY sceneYChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validityChanged)
public:
    QDeclarativeTouchPoint(bool qmlReference = true)
        : _id(0),
          _x(0.0),
          _y(0.0),
          _sceneX(0.0),
          _sceneY(0.0),
          _qmlReferenced(qmlReference),
          _valid(!qmlReference)
    {}

    int id() const { return _id; }
    void setId(int id) { _id = id; }

    qreal x() const { return _x; }
    void setX(qreal x) {
        _x = x;
        emit(xChanged());
    }

    qreal y() const { return _y; }
    void setY(qreal y) {
        _y = y;
        emit(yChanged());
    }

    qreal sceneX() const { return _sceneX; }
    void setSceneX(qreal sceneX) {
        _sceneX = sceneX;
        emit(sceneXChanged());
    }

    qreal sceneY() const { return _sceneY; }
    void setSceneY(qreal sceneY) {
        _sceneY = sceneY;
        emit(sceneYChanged());
    }

    bool isQmlReferenced() { return _qmlReferenced; }

    bool isValid() { return _valid; }
    void setValid(bool valid) {
        _valid = valid;
        emit(validityChanged());
    }

Q_SIGNALS:
    void xChanged();
    void yChanged();
    void sceneXChanged();
    void sceneYChanged();
    void validityChanged();

private:
    int _id;
    qreal _x;
    qreal _y;
    qreal _sceneX;
    qreal _sceneY;
    bool _qmlReferenced;
    bool _valid;
};



class QDeclarativeTouchArea : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QList<QObject*> touches READ touches)
    Q_PROPERTY(QList<QObject*> changedTouches READ changedTouches)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeTouchPoint> touchPoints READ touchPoints NOTIFY touchPointsChanged)
    Q_PROPERTY(int minimumTouches READ minimumTouches WRITE setMinimumTouches)
    Q_PROPERTY(int maximumTouches READ maximumTouches WRITE setMaximumTouches)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor NOTIFY scaleFactorChanged)
    Q_PROPERTY(qreal rotationAngle READ rotationAngle NOTIFY rotationAngleChanged)
    Q_PROPERTY(QDeclarativeScriptString keepMouseFocus READ keepMouseFocus WRITE setKeepMouseFocus)

public:
    QDeclarativeTouchArea(QDeclarativeItem *parent=0);
    ~QDeclarativeTouchArea();

    int minimumTouches() const { return _minimumTouches; }
    void setMinimumTouches(int num) { _minimumTouches = num; }
    int maximumTouches() const { return _maximumTouches; }
    void setMaximumTouches(int num) { _maximumTouches = num; }

    qreal scaleFactor() const { return _scaleFactor; }
    qreal rotationAngle() const { return _rotationAngle; }

    QDeclarativeScriptString keepMouseFocus() const { return _keepMouseFocus; }
    void setKeepMouseFocus(const QDeclarativeScriptString &keep) { _keepMouseFocus = keep; }

    QList<QObject*> touches() { return _touches.values(); }
    QList<QObject*> changedTouches() { return _changedTouches.values(); }

    QDeclarativeListProperty<QDeclarativeTouchPoint> touchPoints() {
        return QDeclarativeListProperty<QDeclarativeTouchPoint>(this, 0, QDeclarativeTouchArea::touchPoint_append, QDeclarativeTouchArea::touchPoint_count, QDeclarativeTouchArea::touchPoint_at, 0);
    }

    static void touchPoint_append(QDeclarativeListProperty<QDeclarativeTouchPoint> *list, QDeclarativeTouchPoint* touch) {
        QDeclarativeTouchArea *q = static_cast<QDeclarativeTouchArea*>(list->object);
        q->addTouchPrototype(touch);
    }

    static int touchPoint_count(QDeclarativeListProperty<QDeclarativeTouchPoint> *list) {
        QDeclarativeTouchArea *q = static_cast<QDeclarativeTouchArea*>(list->object);
        return q->_touchPrototypes.count();
    }

    static QDeclarativeTouchPoint* touchPoint_at(QDeclarativeListProperty<QDeclarativeTouchPoint> *list, int index) {
        QDeclarativeTouchArea *q = static_cast<QDeclarativeTouchArea*>(list->object);
        return static_cast<QDeclarativeTouchPoint*>(q->_touchPrototypes[index]);
    }


Q_SIGNALS:
    void touchStart();
    void touchMove();
    void touchEnd();
    void touchPointsChanged();
    void scaleFactorChanged();
    void rotationAngleChanged();

protected:
    bool sceneEvent(QEvent *);
    void updateTouchPoint(QDeclarativeTouchPoint*, const QTouchEvent::TouchPoint*);
    void updatePinch(QList<QTouchEvent::TouchPoint> *touchPoints);
    void addTouchPrototype(QDeclarativeTouchPoint* prototype);
    void addTouchPoint(const QTouchEvent::TouchPoint *p);
    void clearChangedAndReleasedTouches();
    void updateTopTouchArea();
    void updateTouchData(QEvent*);
    bool event(QEvent *event);
    bool sceneEventFilter(QGraphicsItem *i, QEvent *event);

private:
    QMap<int,QObject*> _touchPrototypes;
    QMap<int,QObject*> _touches;
    QMap<int,QObject*> _changedTouches;
    QList<QObject*> _releasedTouches;
    int _minimumTouches;
    int _maximumTouches;
    qreal _scaleFactor;
    qreal _rotationAngle;
    QDeclarativeTouchArea* _parentTouchArea;
    bool _active;
    bool _stealMouse;
    QDeclarativeScriptString _keepMouseFocus;

};

QML_DECLARE_TYPE(QDeclarativeTouchPoint)

#endif // QDECLARATIVETOUCHAREA_H
