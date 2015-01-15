#ifndef BULLET3DBOX_H
#define BULLET3DBOX_H

#include <Qt3D/QGLSceneNode>
#include <Qt3D/QGraphicsRotation3D>

#include <Qt3D/QGLMaterial>

#include "motionstates.h"

#include <QDebug>


class Bullet3DBox
{
public:
    explicit Bullet3DBox(QGLMaterial* material = nullptr);
    ~Bullet3DBox();

    inline btRigidBody* getBtRigidBody() const
    {
        return my_fallRigidBody;
    }

    inline QGLSceneNode* getSceneNode() const
    {
        return my_node;
    }


private slots:


private:
    static QGLSceneNode* getGlobalNode();
    static QGLSceneNode* my_globalNode;
    btCollisionShape* my_fallShape;
    GLNodeMotionState* my_boxMotionState;
    btRigidBody* my_fallRigidBody;

    QGLSceneNode* my_node;
//    bool my_isVisible;
};

#endif // BULLET3DBOX_H
