#include "bulletboxview.h"

#include <math.h>

#include <Qt3D/QGLBuilder>
#include <Qt3D/QGLCube>

#include <Qt3D/QGLPainter>

#include <Qt3D/QPlane3D>

#include <QtCore/QTimer>

#include <QtGui/QWindow>

#include <QSizeF>

#include <QDateTime>

#include <QtGui/QGuiApplication>

#include <Qt3D/QGLFramebufferObjectSurface>

#include <Qt3D/QGLWindowSurface>

#include <QtGui/QKeyEvent>

#include <QDebug>

#define HALF_BOXCONTAINER_SIZE 20
#define HALF_BOXCONTAINER_THICKNESS 5

BulletBoxView::BulletBoxView(const QSurfaceFormat &format, QWindow *parent) :
    QGLView(format,parent),
#ifdef COUNT_FPS
    my_fps(0),
#endif
    my_broadphase(nullptr),
    my_collisionConfiguration(nullptr),
    my_dispatcher(nullptr),
    my_solver(nullptr),
    my_dynamicsWorld(nullptr),
    my_groundShape(nullptr),
    my_groundMotionState(nullptr),
    my_groundRigidBody(nullptr),
    my_groundNode(nullptr),
    my_mat(new QGLMaterial),
    my_shader(nullptr),
    my_subSurface(nullptr),
    my_spinAngle(0.0f),
    my_lastTimeStamp(QDateTime::currentMSecsSinceEpoch())
{
    setSurfaceType(QWindow::OpenGLSurface);

    //Debug Cam
    my_cam1 =  new QGLCamera(this);
    my_cam1->setFarPlane(9999);
    my_cam1->setEye(QVector3D(0,0,120));
    my_cam1->setUpVector(QVector3D(0,1,0));
    //my_cam1->setCenter(QVector3D(0,0,0));
    my_cam1->setCenter(QVector3D(0,0,120));

    my_cam2 = new QGLCamera(this);
    my_cam2->setFarPlane(9999);
    my_cam2->setEye(QVector3D(0,120,0));
    my_cam2->setUpVector(QVector3D(1,0,0));
    my_cam2->setCenter(QVector3D(0,0,0));

    setCamera(my_cam1);

//    camera()->setFarPlane(9000);
//    camera()->setEye(QVector3D(0,-80,-100));
//    camera()->setUpVector(QVector3D(0,1,0));
//    camera()->setCenter(QVector3D(0,-80,0));

    my_shader = new QGLShaderProgramEffect;
    my_shader->setVertexShaderFromFile("../standard.vsh");
    my_shader->setFragmentShaderFromFile("../fractals_julia.fsh");

    my_mat->setTextureUrl(QUrl("qrc://textures/qt.png"));
    my_mat->setColor(QColor(Qt::darkGreen));
    my_mat->setTextureCombineMode(QGLMaterial::Modulate);
    my_mat->setShininess(64);

    initPhysicWorld();
    //spawnGround();
    spawnBoxContainer();
    startPhysicWorld();

}

qint64 BulletBoxView::getDeltaTimeMiliseconds() const
{
    return QDateTime::currentMSecsSinceEpoch() - my_lastTimeStamp;
}

BulletBoxView::~BulletBoxView()
{
    foreach(Bullet3DBox* b, my_nodes)
    {
        my_nodes.remove(my_nodes.indexOf(b));
        delete b;
    }

    delete my_groundShape;
    delete my_groundMotionState;
    delete my_groundRigidBody;

    delete my_dynamicsWorld;
    delete my_solver;
    delete my_dispatcher;
    delete my_collisionConfiguration;
    delete my_broadphase;
}

void BulletBoxView::initPhysicWorld()
{
    //World
    my_broadphase = new btDbvtBroadphase();
    my_collisionConfiguration = new btDefaultCollisionConfiguration();
    my_dispatcher = new btCollisionDispatcher(my_collisionConfiguration);
    my_solver = new btSequentialImpulseConstraintSolver;
    my_dynamicsWorld = new btDiscreteDynamicsWorld(my_dispatcher,my_broadphase,my_solver,my_collisionConfiguration);
    my_dynamicsWorld->setGravity(btVector3(0,-10,0));

    my_dynamicsWorld->setInternalTickCallback(&BulletBoxView::physicWorldTickCallBack);

}

void BulletBoxView::startPhysicWorld()
{

    QTimer* stepClock = new QTimer(this);
    connect(stepClock,SIGNAL(timeout()),SLOT(stepPhysicalWorld()));
    stepClock->start(1);

    QTimer* spawner = new QTimer(this);
    connect(spawner,SIGNAL(timeout()),SLOT(spawnBox()));
    spawner->start(0);

#ifdef COUNT_FPS
    QTimer* counter = new QTimer(this);
    connect(counter,SIGNAL(timeout()),SLOT(resetFps()));
    counter->start(1000);
#endif
}

void BulletBoxView::stepPhysicalWorld()
{
    float dt = float(getDeltaTimeMiliseconds()) * 0.001;
    my_lastTimeStamp = QDateTime::currentMSecsSinceEpoch();
    my_dynamicsWorld->stepSimulation(dt,5);
    spinBoxContainer();
    update();
}

void BulletBoxView::spinBoxContainer(float dt)
{
    if(my_spinAngle == M_PI)
    {
        qDebug() << __PRETTY_FUNCTION__ << "REVOLUTION";
        my_spinAngle = 0.0f;
    }
    my_spinAngle += M_PI_4/90.0;
    btQuaternion q(btVector3(0,0,1),my_spinAngle);
    my_kinematicMotionState->setTransform(btTransform(q));
}

void BulletBoxView::spawnBoxContainer()
{
    btCollisionShape* staticboxShape1 =
            new btBoxShape(btVector3(HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_THICKNESS*2,HALF_BOXCONTAINER_SIZE));//floor
    btCollisionShape* staticboxShape2 =
            new btBoxShape(btVector3(HALF_BOXCONTAINER_THICKNESS*2,HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_SIZE));//left wall
    btCollisionShape* staticboxShape3 =
            new btBoxShape(btVector3(HALF_BOXCONTAINER_THICKNESS*2,HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_SIZE));//right wall
    btCollisionShape* staticboxShape4 =
            new btBoxShape(btVector3(HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_THICKNESS*2));//front wall
    btCollisionShape* staticboxShape5 =
            new btBoxShape(btVector3(HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_THICKNESS*2));//back wall
    btCollisionShape* staticboxShape6 =
            new btBoxShape(btVector3(HALF_BOXCONTAINER_SIZE,HALF_BOXCONTAINER_THICKNESS*2,HALF_BOXCONTAINER_SIZE));//top

    btCompoundShape* compoundShape = new btCompoundShape();
    float mass = 10.0f;
    btVector3 inertia(0, 0, 0);
    btTransform startTransform;
    startTransform.setIdentity();

    startTransform.setOrigin(btVector3(0,-HALF_BOXCONTAINER_SIZE - HALF_BOXCONTAINER_THICKNESS,0));
    compoundShape->addChildShape(startTransform,staticboxShape1);
    startTransform.setOrigin(btVector3(-HALF_BOXCONTAINER_SIZE - HALF_BOXCONTAINER_THICKNESS,0,0));
    compoundShape->addChildShape(startTransform,staticboxShape2);
    startTransform.setOrigin(btVector3(HALF_BOXCONTAINER_SIZE + HALF_BOXCONTAINER_THICKNESS,0,0));
    compoundShape->addChildShape(startTransform,staticboxShape3);
    startTransform.setOrigin(btVector3(0,0,-HALF_BOXCONTAINER_SIZE - HALF_BOXCONTAINER_THICKNESS));
    compoundShape->addChildShape(startTransform,staticboxShape4);
    startTransform.setOrigin(btVector3(0,0,HALF_BOXCONTAINER_SIZE + HALF_BOXCONTAINER_THICKNESS));
    compoundShape->addChildShape(startTransform,staticboxShape5);
    startTransform.setOrigin(btVector3(0,HALF_BOXCONTAINER_SIZE + HALF_BOXCONTAINER_THICKNESS,0));
    compoundShape->addChildShape(startTransform,staticboxShape6);

    startTransform.setOrigin(btVector3(0,0,0));

    my_kinematicMotionState = new KinematicMotionState(btTransform(btQuaternion(0,0,0,1)));
    compoundShape->calculateLocalInertia(mass,inertia);
    btRigidBody::btRigidBodyConstructionInfo bodyCI(mass, my_kinematicMotionState, compoundShape, inertia);
    body = new btRigidBody(bodyCI);
    body->setCollisionFlags(body->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
    body->setActivationState(DISABLE_DEACTIVATION);
    body->setGravity(btVector3(0,0,0));

    my_dynamicsWorld->addRigidBody(body);
}

void BulletBoxView::spawnBox()
{
    if(my_nodes.size() >= 150) return;
    Bullet3DBox* box = new Bullet3DBox(my_mat);
    box->getSceneNode()->setUserEffect(my_shader);
    my_nodes.append(box);
    my_dynamicsWorld->addRigidBody(box->getBtRigidBody());
}

void BulletBoxView::spawnGround()
{
    my_groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
    my_groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-80,0)));
    btRigidBody::btRigidBodyConstructionInfo
                    groundRigidBodyCI(0,my_groundMotionState,my_groundShape,btVector3(0,0,0));
    my_groundRigidBody = new btRigidBody(groundRigidBodyCI);
    my_dynamicsWorld->addRigidBody(my_groundRigidBody);

    my_groundRigidBody->setFriction(0);
    my_groundRigidBody->setRestitution(0);

    QGLBuilder builder;
    builder.addPane(QSizeF(100,100));
    my_groundNode = builder.finalizedSceneNode();

    QGraphicsRotation3D* rot = new QGraphicsRotation3D;
    rot->setAngle(-90);
    rot->setOrigin(QVector3D(0,0,0));
    rot->setAxis(QVector3D(1,0,0));
    my_groundNode->addTransform(rot);

    QMatrix4x4 matrix;
    matrix.translate(0,-80,0);
    my_groundNode->setLocalTransform(matrix);

    QGLMaterial *mat = new QGLMaterial;
    mat->setDiffuseColor(Qt::white);
    my_groundNode->setMaterial(mat);
    my_groundNode->setEffect(QGL::FlatColor);
}

void BulletBoxView::physicWorldTickCallBack(btDynamicsWorld *world, btScalar timeStep)
{
    //qDebug() << __PRETTY_FUNCTION__ << timeStep;
}

void BulletBoxView::depthSortVisibleObjects(QGLPainter* painter)
{
#ifdef COUNT_TIME
    qint64 before = QDateTime::currentDateTime().currentMSecsSinceEpoch();
#endif
    my_zSortedVisibleGLNodes.clear();
    QMatrix4x4 modelViewMatrix = camera()->modelViewMatrix();
    foreach(Bullet3DBox* box, my_nodes)
    {
        QGLSceneNode* node = box->getSceneNode();
        if(!painter->isCullable(node->boundingBox()))
            my_zSortedVisibleGLNodes.insert(modelViewMatrix.map(node->position()).z(), node);
    }
#ifdef COUNT_TIME
    qDebug() << __PRETTY_FUNCTION__ << "Watching visible/total nodes" << my_zSortedVisibleGLNodes.size() << "/" << my_nodes.size();
    qDebug() << __PRETTY_FUNCTION__ << "Time for sorting objects in ms "<< (QDateTime::currentDateTime().currentMSecsSinceEpoch() - before);
#endif
}

void BulletBoxView::initializeGL(QGLPainter *painter)
{
    qDebug() << __PRETTY_FUNCTION__;
    QGLView::initializeGL(painter);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}

void BulletBoxView::resizeGL(int w, int h)
{
    QGLView::resizeGL(w,h);
    if(my_subSurface)
        my_subSurface->setRegion(QRect(QPoint(0,0), QSize(w,h)/2));
}

void BulletBoxView::keyPressEvent(QKeyEvent *e)
{
    qDebug() << __PRETTY_FUNCTION__ << e->key();
    switch(e->key())
    {
    case Qt::Key_Up:
        camera()->translateEye(0,0,1);
        camera()->translateCenter(0,0,1);
        //newEye = camera()->center() + inverseModelView.map(QVector3D(0,0,-1));
        break;
    case Qt::Key_Down:
        camera()->translateEye(0,0,-1);
        camera()->translateCenter(0,0,-1);
        //newEye = camera()->center() + inverseModelView.map(QVector3D(0,0,1));
        break;
    case Qt::Key_Left:
        camera()->translateEye(-1,0,0);
        camera()->translateCenter(-1,0,0);
        //newEye  = camera()->center() + inverseModelView.map(QVector3D(-1,0,0));
        break;
    case Qt::Key_Right:
        camera()->translateEye(1,0,0);
        camera()->translateCenter(1,0,0);
        //newEye = camera()->center() + inverseModelView.map(QVector3D(1,0,0));
        break;
    case Qt::Key_A:
        camera()->translateEye(0,1,0);
        camera()->translateCenter(0,1,0);
        //newEye = camera()->center() + inverseModelView.map(QVector3D(0,1,0));
        break;
    case Qt::Key_Q:
        camera()->translateEye(0,-1,0);
        camera()->translateCenter(0,-1,0);
        //newEye = camera()->center() + inverseModelView.map(QVector3D(0,-1,0));
        break;
    default:
        qDebug() << __PRETTY_FUNCTION__ << "ignoring KeyEvent";
        e->ignore();
        return;
    }
    qDebug() << __PRETTY_FUNCTION__ << camera()->eye();
    e->accept();
}

void BulletBoxView::keyReleaseEvent(QKeyEvent *e)
{
    qDebug() << __PRETTY_FUNCTION__;
    e->accept();
}

void BulletBoxView::earlyPaintGL(QGLPainter *painter)
{
    //glClearColor(255,255,255,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(!my_subSurface)
        my_subSurface = new QGLSubsurface(painter->currentSurface(),QRect(QPoint(0,0),QWindow::size()/2));

    depthSortVisibleObjects(painter);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);
}

void BulletBoxView::paintGL(QGLPainter *painter)
{
#ifdef COUNT_TIME
    qint64 before = QDateTime::currentDateTime().currentMSecsSinceEpoch();
#endif

    drawMainSurface(painter);
    drawSubSurface(painter);

#ifdef COUNT_TIME
    qDebug() << __PRETTY_FUNCTION__ << "Time for painting objects in ms "<< (QDateTime::currentDateTime().currentMSecsSinceEpoch() - before);
#endif
#ifdef COUNT_FPS
    ++my_fps;
#endif
}

void BulletBoxView::drawMainSurface(QGLPainter *painter)
{
    QMapIterator<qreal, QGLSceneNode*> i(my_zSortedVisibleGLNodes);
    while(i.hasNext()) i.next().value()->draw(painter);
}

void BulletBoxView::drawSubSurface(QGLPainter *painter)
{
    painter->setCamera(my_cam2);
    painter->pushSurface(my_subSurface);
    QMapIterator<qreal, QGLSceneNode*> i(my_zSortedVisibleGLNodes);
    while(i.hasNext()) i.next().value()->draw(painter);
    painter->popSurface();
}

#ifdef COUNT_FPS
void BulletBoxView::resetFps()
{
    qDebug() << __PRETTY_FUNCTION__ << my_fps;
    my_fps = 0;
}
#endif

