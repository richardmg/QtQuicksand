#include <QtWidgets/QApplication>
#include <QtGui/QScreen>
#include <QtGui/QPainter>
#include <QtGui/QImageReader>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtGui/QTouchEvent>
#include <math.h>

#include "sandboxitem.h"

SandBoxItem::SandBoxItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    // Important, otherwise the paint method is never called
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptTouchEvents(true);

    image = QImage(QGuiApplication::primaryScreen()->size(), QImage::Format_RGB32);

    // startup the sandSimulation
    m_sandBox = new CVSandBox(image.width(), image.height());
    m_sandBox->gravity().set(0, 0);
    m_sandBox->setLight(FP_VAL * 200, FP_VAL * 200);

    m_selectedTool = eSANDTOOL_PUSH_TOWARDS;
    m_paintingSize = 4;
    m_tangle = 0.0f;

    // UI update timer (ms)
    m_fastUpdateTimer = this->startTimer(16); // max - framerate of 65535

    m_sandBox->autoPaintToResetTarget(4, 0);
    m_intro = new CVSand_Intro(false);
    m_inactivityCounter = 0;

#if defined ENABLE_SENSORS
    // Sensor and reading timer (ms)
    m_sensor = new QAccelerometer(this);
    m_sensor->start();
    m_sensor_tid = 0;
    m_sensor_tid = this->startTimer(100);
#endif
}

SandBoxItem::~SandBoxItem()
{
    if (m_intro) delete m_intro;
    if (m_sandBox) delete m_sandBox;

#if defined ENABLE_SENSORS
    m_sensor->stop();
#endif
}

void SandBoxItem::autopaint(QImage target)
{
    CTSurface_RGBA8888 apt;
    apt.setAsReference(target.bits(), target.width(), target.height(), target.bytesPerLine() / 4, eTSURFACEFORMAT_RGBA8888);
    m_sandBox->autoPaintTo(apt);
}

void SandBoxItem::toolStep(int fixedx, int fixedy, SandBrush *brush, int *id)
{
    switch (brush->tool())
    {
    case eSANDTOOL_SCRATCH_TO_TARGET:
        m_sandBox->getGrid()->scratch_to_target(fixedx, fixedy, (m_paintingSize + 3) * FP_VAL, FP_VAL / 2, m_sandBox->getAutoPaintTargetMem(), id);
        break;
    case eSANDTOOL_ADD:
        m_sandBox->getGrid()->addMass(fixedx, fixedy, (m_paintingSize + 4) * FP_VAL, FP_VAL / 16, id);
        break;
    case eSANDTOOL_CURVE:
        m_sandBox->getGrid()->curveMass(fixedx, fixedy, (m_paintingSize) * FP_VAL, FP_VAL);
        break;
    case eSANDTOOL_PUSH:
        m_sandBox->getGrid()->push(fixedx, fixedy, (m_paintingSize + 1) * FP_VAL, FP_VAL * 2, id);
        break;
    case eSANDTOOL_PUSH_TOWARDS:
        m_sandBox->getGrid()->push_towards(fixedx, fixedy, id[0], id[1], (m_paintingSize + 3) * FP_VAL, FP_VAL * 2);
        break;
    case  eSANDTOOL_PULL:
        m_sandBox->getGrid()->pull(fixedx + (int)(cosf(m_tangle) * (20000.0f * (float)m_paintingSize)),
                                   fixedy + (int)(sinf(m_tangle) * (20000.0f * (float)m_paintingSize)),
                                   m_paintingSize * FP_VAL * 2, FP_VAL * 3 / 2);
        break;
    case eSANDTOOL_LIGHT:
        m_sandBox->setLight((fixedx - ((image.width() << 13) >> SANDGRID_DIV_BITS)) / 8,
                            (fixedy - ((image.height() << 13) >> SANDGRID_DIV_BITS)) / 8);
        break;
    default:
        break;
    }
}

void SandBoxItem::newSandFrame()
{
    int id[2];

    foreach (SandBrush *brush, m_sandBrushes.values())
    {
        if (brush->tool() != eSANDTOOL_NONE)
        {
            m_inactivityCounter = 0;

            id[0] = (brush->paintingPos().x() - brush->exPaintingPos().x());
            id[1] = (brush->paintingPos().y() - brush->exPaintingPos().y());

            int l = tmath_sqrt((id[0] >> 14) * (id[0] >> 14) + (id[1] >> 14) * (id[1] >> 14));
            int divi = (m_paintingSize / 2);
            if (divi < 1) divi = 1;
            int steps = l / divi + 1;
            if (brush->tool() == eSANDTOOL_LIGHT) steps = 1;

            for (int s = 0; s < steps; s++)
            {
                toolStep(brush->exPaintingPos().x() + id[0] * s / steps, brush->exPaintingPos().y() + id[1] * s / steps, brush, id);
            }

            // udpate paintingpos
            brush->setExPaintingPos(brush->paintingPos());
        }
    }
    m_tangle += 0.5f;
    m_sandBox->run(65536 >> 6); // 64fps
}

void SandBoxItem::timerEvent(QTimerEvent *e)
{
#if defined ENABLE_SENSORS
    if (e->timerId() == m_sensor_tid)
    {
        readSensor();
        return;
    }
#endif

    if (e->timerId() != m_fastUpdateTimer) return;

    CTSurface_RGBA8888 renderRef;
    renderRef.setAsReference(image.bits(), image.width(), image.height(), image.bytesPerLine() / 4, eTSURFACEFORMAT_RGBA8888);

    // run intro instead of anything else
    if (m_intro)
    {
        int width = 320;
        int height = 200;

        CTSurface_RGBA8888 sub;
        sub.setReference(&renderRef, renderRef.getWidth() / 2 - width, renderRef.getHeight() / 2 - height, width * 2, height * 2);

        if (m_intro->run(1024) == 0)
        {
            delete m_intro;
            m_intro = 0;
            if (m_inactivityCounter > 0)
            {
                m_sandBox->markAllChanged();
                m_sandBox->render(renderRef);
                update(); // just to prevent the full-update after first intro
            }
            emit showToolBar();
            return;
        }

        m_intro->draw(&sub);
        update(QRect(renderRef.getWidth() / 2 - width, renderRef.getHeight() / 2 - height, width * 2, height * 2));
        return;
    }

    if (m_inactivityCounter > 1000)
    {
        m_inactivityCounter = 1;
        m_intro = new CVSand_Intro(true);
        update();
    }

    newSandFrame();

    int bounds[4];
    // draw the sandbox into m_image
    if (m_sandBox->render(renderRef, bounds) != 0)
    {
        update(QRect(bounds[0], bounds[2], (bounds[1] - bounds[0]) + SANDGRID_DIV, (bounds[3] - bounds[2]) + SANDGRID_DIV));
    }
}

void SandBoxItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    beginToolUse(0, e->scenePos());
}

void SandBoxItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    updateToolUse(0, e->scenePos());
}

void SandBoxItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    endToolUse(0);
}

bool SandBoxItem::sceneEvent(QEvent *event)
{
    QTouchEvent *touchEvent = 0;
    switch (event->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            touchEvent = static_cast<QTouchEvent *>(event);
            foreach (QTouchEvent::TouchPoint point, touchEvent->touchPoints()) {

                if (point.state() & Qt::TouchPointPressed) {
                    beginToolUse(point.id(), point.scenePos());
                    event->accept();
                }

                if (point.state() & Qt::TouchPointMoved) {
                    updateToolUse(point.id(), point.scenePos());
                    event->accept();
                }
                if (point.state() & Qt::TouchPointReleased) {
                    endToolUse(point.id());
                    event->accept();
                }
            }
            return true;
    case QEvent::TouchCancel:
        qDebug() << "Cancle!";
        default:
            break;
    }

    return QDeclarativeItem::event(event);
}

void SandBoxItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->drawImage(0, 0, image);
}

#define SHAKE_LIMIT 310000

void SandBoxItem::readSensor()
{
#if defined ENABLE_SENSORS
    // Read rotation data
    QAccelerometerReading *reading = m_sensor->reading();

    // up/down rotation
    qreal x = reading->property("x").value<qreal>();
    qreal y = reading->property("y").value<qreal>();
    qreal z = reading->property("z").value<qreal>();

    int vx = (int)(x * (float)FP_VAL);
    int vy = (int)(y * (float)FP_VAL);
    int vz = (int)(z * (float)FP_VAL);

    int dif_amount = tmath_fpmul(vx, vx) + tmath_fpmul(vy, vy) + tmath_fpmul(vz, vz);

    dif_amount = tmath_fpsqrt(dif_amount);

    if (dif_amount > SHAKE_LIMIT)
    {
        dif_amount -= SHAKE_LIMIT;
        m_sandBox->shake(vx, vy);
    };
#endif
}

void SandBoxItem::beginToolUse(int pointId, const QPointF &pos)
{
    if (m_intro)
        m_intro->cancel();

    m_inactivityCounter = 1;

    if (m_sandBrushes.value(pointId, 0)) {
        return;
    }

    int x = ((int)pos.x() << FP_BITS) >> SANDGRID_DIV_BITS;
    int y = ((int)pos.y() << FP_BITS) >> SANDGRID_DIV_BITS;
    if (x >= 0 && y >= 0 && x < (m_sandBox->getGrid()->width() << FP_BITS) && y < (m_sandBox->getGrid()->height() << FP_BITS))
    {
        eSANDTOOL tool;

        if (m_sandBox->isScratching() == false)
            tool = m_selectedTool;
        else
            tool = eSANDTOOL_SCRATCH_TO_TARGET;

        m_sandBrushes.insert(pointId, new SandBrush(m_selectedTool, QPoint(x, y)));
    }
    emit hideToolBar();
}

void SandBoxItem::updateToolUse(int pointId, const QPointF &pos)
{
    SandBrush *brush = m_sandBrushes.value(pointId, 0);

    if (brush && brush->tool() != eSANDTOOL_NONE)
    {
        QPoint newPos = QPoint(((int)pos.x() << FP_BITS) >> SANDGRID_DIV_BITS,
                               ((int)pos.y() << FP_BITS) >> SANDGRID_DIV_BITS);
        brush->setPaintingPos(newPos);
    }
}

void SandBoxItem::endToolUse(int pointId)
{
    delete m_sandBrushes.value(pointId);
    m_sandBrushes.remove(pointId);
    emit showToolBar();
}

void SandBoxItem::setPen(int value)
{
    m_inactivityCounter = 1;
    m_paintingSize = value;
}

void SandBoxItem::setTool(SANDTOOL type)
{
    m_inactivityCounter = 1;
    m_selectedTool = (eSANDTOOL)type;
}

void SandBoxItem::reset()
{
    m_inactivityCounter = 1;
    m_sandBox->autoPaintToResetTarget(rand() & 5);
}

void SandBoxItem::open(const QString &fileName)
{
    m_inactivityCounter = 1;
    if (fileName.isEmpty()) return;

    // Load image by QImageReader
    QImageReader *reader = new QImageReader();
    reader->setFileName(fileName);

    // Scale size to fit into screen
    QSize imageSize = reader->size();
    imageSize.scale(width(), height(), Qt::KeepAspectRatio);
    reader->setScaledSize(imageSize);

    // Reads image
    QImage loadedImage = reader->read();
    delete reader;

    // Sand image
    if (!loadedImage.isNull())
    {
        m_sandBox->markAllChanged();
        autopaint(loadedImage);
        update();
    }
}
