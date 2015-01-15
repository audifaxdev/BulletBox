#include "bullet3dbox.h"
#include <Qt3D/QGLBuilder>
#include <Qt3D/QGLCube>

#define HALF_CUBE_SIZE 2

QGLSceneNode* Bullet3DBox::my_globalNode = nullptr;

QGLSceneNode *Bullet3DBox::getGlobalNode()
{
    if(!my_globalNode)
    {
        QGLBuilder builder;
        builder << QGL::Faceted << QGLCube(HALF_CUBE_SIZE*2);
        return (my_globalNode = builder.finalizedSceneNode() );
    }else
        return my_globalNode->clone();
}

Bullet3DBox::Bullet3DBox(QGLMaterial *material) :
    my_fallShape(nullptr),
    my_boxMotionState(nullptr),
    my_fallRigidBody(nullptr)
{
    my_node = getGlobalNode();

    if(material) my_node->setMaterial(material);

    QVector3D initPos(0,0,0);

    my_node->setEffect(QGL::LitModulateTexture2D);
    my_node->setPosition(initPos);
    btScalar mass = 1.0f;
    btVector3 fallInertia(0,0,0);
    my_fallShape = new btBoxShape(btVector3(HALF_CUBE_SIZE,HALF_CUBE_SIZE,HALF_CUBE_SIZE));
    my_fallShape->calculateLocalInertia(mass,fallInertia);

    my_boxMotionState = new GLNodeMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(initPos.x(),initPos.y(),initPos.z())), my_node);
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,my_boxMotionState,my_fallShape,fallInertia);
    my_fallRigidBody = new btRigidBody(fallRigidBodyCI);
    my_fallRigidBody->setFlags(my_fallRigidBody->getFlags() | DISABLE_DEACTIVATION);
}

Bullet3DBox::~Bullet3DBox()
{
    delete my_node;
    delete my_fallRigidBody;
    delete my_boxMotionState;
    delete my_fallShape;
}
