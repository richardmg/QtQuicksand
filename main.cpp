#include "sandboxitem.h"
#include "qdeclarativetoucharea.h"

#include <QtWidgets/QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtOpenGL/QGLWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<SandBoxItem>("QuickSandModel", 1, 0, "SandBox");
    qmlRegisterType<QDeclarativeTouchArea>("Touch", 1, 0, "TouchArea");
    qmlRegisterType<QDeclarativeTouchPoint>("Touch", 1, 0, "TouchPoint");

    QDeclarativeView viewer;
    viewer.setSource(QUrl("qrc:/main.qml"));
    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);

    QGLFormat format = QGLFormat(QGL::DirectRendering);
    format.setSampleBuffers(false);
    QGLWidget *glWidget = new QGLWidget(format);
    glWidget->setAutoFillBackground(false);
    viewer.setViewport(glWidget);
    viewer.setGeometry(0, 0, 1280, 720);
    //viewer.showFullScreen();
    viewer.show();

    return app.exec();
}
