#include <QtGui/QGuiApplication>
#include "bulletboxview.h"
#include <QtGui/QWindow>
#include <QtGui/QSurfaceFormat>

int main(int argc, char *argv[])
{
    QGuiApplication a(argc, argv);

    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setDepthBufferSize(24);
    format.setSamples(16);

    BulletBoxView bulletBox(format);
    bulletBox.resize(1280,1024);
    bulletBox.show();

    return a.exec();
}
