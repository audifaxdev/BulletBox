#ifndef BULLETBOXVIEW_H
#define BULLETBOXVIEW_H

#include <QVector>
#include <QMultiMap>

#include <Qt3D/QGLView>
#include <QtGui/QOpenGLFunctions>
#include <Qt3D/QGLSceneNode>
#include <Qt3D/QGLMaterial>
#include <Qt3D/QGLShaderProgramEffect>
#include <Qt3D/QGLSubsurface>

#include <bullet/btBulletDynamicsCommon.h>

#include "bullet3dbox.h"
#include "motionstates.h"

class BulletBoxView : public QGLView, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit BulletBoxView(const QSurfaceFormat &format, QWindow *parent = 0);
    qint64 getDeltaTimeMiliseconds() const;
    ~BulletBoxView();
signals:
    
public slots:

    void spawnBox();
    void spawnGround();
    void stepPhysicalWorld();

protected:
    virtual void initializeGL(QGLPainter *painter);
    virtual void earlyPaintGL(QGLPainter *painter);
    virtual void paintGL(QGLPainter *painter);
    virtual void resizeGL(int w, int h);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);

    void drawMainSurface(QGLPainter*);
    void drawSubSurface(QGLPainter*);

private slots:
#ifdef COUNT_FPS
    void resetFps();
#endif
private:
    void initPhysicWorld();
    void startPhysicWorld();
    void spawnBoxContainer();
    void spinBoxContainer(float deltaTime = 1.0f/90.0f);
    static void physicWorldTickCallBack(btDynamicsWorld *world, btScalar timeStep);

    void depthSortVisibleObjects(QGLPainter*);

    //Camera

    QGLCamera* my_cam1;
    QGLCamera* my_cam2;

    //Viewports
    QGLSubsurface* my_subSurface;

    //World
    btBroadphaseInterface* my_broadphase;
    btDefaultCollisionConfiguration* my_collisionConfiguration;
    btCollisionDispatcher* my_dispatcher;
    btSequentialImpulseConstraintSolver* my_solver;
    btDiscreteDynamicsWorld* my_dynamicsWorld;

    //Ground
    btCollisionShape* my_groundShape;
    btDefaultMotionState* my_groundMotionState;
    btRigidBody* my_groundRigidBody;

    //Nodes Unsorted/sorted containers
    QVector<Bullet3DBox*> my_nodes;

    QMultiMap<qreal,QGLSceneNode*> my_zSortedVisibleGLNodes;

    //Materials & shaders
    QGLSceneNode* my_groundNode;
    QGLSceneNode* my_spiningBoxNode;
    KinematicMotionState* my_kinematicMotionState;
    QGLMaterial* my_mat;
    QGLTexture2D* my_text;
    QGLShaderProgramEffect* my_shader;
#ifdef COUNT_FPS
    unsigned short my_fps;
#endif
    btScalar my_spinAngle;
    qint64 my_lastTimeStamp;
    btRigidBody* body;
};

#endif // BULLETBOXVIEW_H
