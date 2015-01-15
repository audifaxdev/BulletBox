#include "bulletboxviewport.h"

#include <QDebug>

BulletBoxViewport::BulletBoxViewport(QVector<Bullet3DBox *> &nodes, const QSurfaceFormat& format, QWindow *parent) :
    QGLView(format,parent),
    my_nodes(nodes)
{
    setSurfaceType(QWindow::OpenGLSurface);
    //Debug Cam
    camera()->setFarPlane(9999);
    camera()->setEye( QVector3D(120,0,0) );
    camera()->setUpVector( QVector3D(0,1,0) );
    camera()->setCenter( QVector3D(0,0,0) );
}

void BulletBoxViewport::initializeGL(QGLPainter *painter)
{
    QGLView::initializeGL(painter);
}

void BulletBoxViewport::earlyPaintGL(QGLPainter *painter)
{
    QGLView::earlyPaintGL(painter);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);
}

void BulletBoxViewport::paintGL(QGLPainter *painter)
{
    foreach(Bullet3DBox* box, my_nodes)
    {
        box->getSceneNode()->draw(painter);
        qDebug() << __PRETTY_FUNCTION__ << box;
    }
}
