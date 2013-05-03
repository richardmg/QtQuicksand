#ifndef QUICKITEM_H
#define QUICKITEM_H

#include <QtDeclarative/QDeclarativeItem>
#include <QtGui/QPainter>
#include <QtGui/QImage>
#include <QtCore/QMap>

#include "vsandbox.h"
#include "vsand_intro.h"

#if defined ENABLE_SENSORS
#include <QAccelerometer>
#include <QAccelerometerReading>
#endif

class SandBoxItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_ENUMS(SANDTOOL)

public:
    enum SANDTOOL
    {
        SANDTOOL_NONE,
        SANDTOOL_SCRATCH_TO_TARGET,
        SANDTOOL_CURVE,
        SANDTOOL_ADD,
        SANDTOOL_PUSH,
        SANDTOOL_PUSH_TOWARDS,
        SANDTOOL_PULL,
        SANDTOOL_GRAVITY,
        SANDTOOL_LIGHT
    };

public:
    SandBoxItem(QDeclarativeItem *parent = 0);
    ~SandBoxItem();

public:
    Q_INVOKABLE void setPen(int value);
    Q_INVOKABLE void setTool(SANDTOOL type);
    Q_INVOKABLE void reset();
    Q_INVOKABLE void open(const QString &fileName);

signals:
    void showToolBar();
    void hideToolBar();

protected:
    void timerEvent(QTimerEvent *);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    bool sceneEvent(QEvent *);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    class SandBrush {
    public:
        SandBrush(eSANDTOOL tool, const QPoint &pos)
            : m_paintingTool(tool)
            , m_paintingPos(pos)
            , m_expaintingPos(pos)
        { }

        void setTool(eSANDTOOL type) { m_paintingTool = type; }
        eSANDTOOL tool() { return m_paintingTool; }

        QPoint paintingPos() { return m_paintingPos; }
        void setPaintingPos(const QPoint &pos) { m_paintingPos = pos; }

        QPoint exPaintingPos() { return m_expaintingPos; }
        void setExPaintingPos(const QPoint &pos) { m_expaintingPos = pos; }

    private:
        eSANDTOOL m_paintingTool;
        QPoint m_paintingPos;
        QPoint m_expaintingPos;
    };

    void beginToolUse(int pointId, const QPointF &pos);
    void updateToolUse(int pointId, const QPointF &pos);
    void endToolUse(int pointId);
    void newSandFrame();
    void toolStep(int fixedx, int fixedy, SandBrush *brush, int *id);
    void autopaint(QImage target);
    void readSensor();

    QImage          image;
    int             m_fastUpdateTimer;
    int             m_inactivityCounter;

    // intro specific
    CVSand_Intro*   m_intro;

    // SandBox-specific
    CVSandBox*      m_sandBox;
    eSANDTOOL       m_selectedTool;
    eSANDTOOL       m_selectedSandTool;
    int             m_paintingSize;
    float           m_tangle;

    QMap<int, SandBrush *>  m_sandBrushes;

#if defined ENABLE_SENSORS
    QAccelerometer* m_sensor;
    int             m_sensor_tid;
#endif
};

QML_DECLARE_TYPE(SandBoxItem)

#endif // QUICKITEM_H
