#include "motionstates.h"

GLNodeMotionState::GLNodeMotionState(const btTransform &initialpos, QGLSceneNode *node) :
    my_pos(initialpos),
    my_node(node)
{
    Q_ASSERT(my_node);
    setWorldTransform(my_pos);
}

GLNodeMotionState::~GLNodeMotionState()
{
}

void GLNodeMotionState::getWorldTransform(btTransform &worldTrans) const
{
    worldTrans = my_pos;
}

void GLNodeMotionState::setWorldTransform(const btTransform &worldTrans)
{
    my_pos = worldTrans;
    btVector3 pos = worldTrans.getOrigin();
    my_node->setPosition(QVector3D(pos.x(),pos.y(),pos.z()));

    btQuaternion r = worldTrans.getRotation();
    QQuaternion q(r.getW(),r.x(),r.y(),r.z());
    QMatrix4x4 rotation;
    rotation.rotate(q);
    my_node->setLocalTransform(rotation);
}


KinematicMotionState::KinematicMotionState(const btTransform &initialpos) :
    my_pos(initialpos)
{
}

KinematicMotionState::~KinematicMotionState()
{
}

void KinematicMotionState::getWorldTransform(btTransform &worldTrans) const
{
    worldTrans = my_pos;
}

void KinematicMotionState::setWorldTransform(const btTransform &worldTrans)
{
    my_pos = worldTrans;
}

void KinematicMotionState::setTransform(const btTransform &transform)
{
    my_pos = transform;
}
