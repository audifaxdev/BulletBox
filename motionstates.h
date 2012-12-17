#ifndef GLNODEMOTIONSTATE_H
#define GLNODEMOTIONSTATE_H
#include <bullet/btBulletDynamicsCommon.h>
#include <Qt3D/QGLSceneNode>

class KinematicMotionState : public btMotionState
{
public:
    KinematicMotionState(const btTransform &initialpos);
    virtual ~KinematicMotionState();
    virtual void getWorldTransform(btTransform &worldTrans) const;
    virtual void setWorldTransform(const btTransform &worldTrans);
    virtual void setTransform(const btTransform &transform);
private:
    btTransform my_pos;
};

class GLNodeMotionState : public btMotionState
{
public:
    GLNodeMotionState(const btTransform &initialpos, QGLSceneNode *node);
    virtual ~GLNodeMotionState();
    virtual void getWorldTransform(btTransform &worldTrans) const;
    virtual void setWorldTransform(const btTransform &worldTrans);

private:
    btTransform my_pos;
    QGLSceneNode* my_node;
};

#endif // GLNODEMOTIONSTATE_H
