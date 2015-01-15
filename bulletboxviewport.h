#ifndef BULLETBOXVIEWPORT_H
#define BULLETBOXVIEWPORT_H

#include <QGLView>
#include "bullet3dbox.h"

class BulletBoxViewport : public QGLView, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit BulletBoxViewport(QVector<Bullet3DBox*> &nodes, const QSurfaceFormat&, QWindow *parent = 0);
    
signals:
    
public slots:
    
protected:
    virtual void initializeGL(QGLPainter *painter);
    virtual void earlyPaintGL(QGLPainter *painter);
    virtual void paintGL(QGLPainter *painter);

private:
    QVector<Bullet3DBox*> &my_nodes;
};

#endif // BULLETBOXVIEWPORT_H
