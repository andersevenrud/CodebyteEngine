/*********************************************************************************
Codebyte Engine

Copyright (c) 2014 Cameron Kline

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.
*********************************************************************************/

#include "Physics.h"

/********************** Static code **********************/
btBroadphaseInterface*                 Physics::broadphase;
btDefaultCollisionConfiguration*       Physics::collisionConfiguration;
btCollisionDispatcher*                 Physics::dispatcher;
btSequentialImpulseConstraintSolver*   Physics::solver;
btDiscreteDynamicsWorld*               Physics::dynamicsWorld;
std::vector<Physics*> Physics::physicsObjects;
ulong Physics::nextID = 0;
float Physics::timeStep;

void Physics::Setup(glm::vec3 gravity, float fixedTimeStep){
    timeStep = fixedTimeStep;
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}
void Physics::Update(){
    dynamicsWorld->stepSimulation(timeStep, 10);
}
Physics* Physics::At(ulong id){
    for(uint i = 0; i < (uint)physicsObjects.size(); i++){
        if(physicsObjects.at(i)->id == id) return physicsObjects.at(i);
    }
    return NULL;
}
Physics* Physics::At(String name){
    for(uint i = 0; i < (uint)physicsObjects.size(); i++){
        if(physicsObjects.at(i)->name == name) return physicsObjects.at(i);
    }
    std::cout << "Physics Object with name: '" << name << "' does not exist!" << std::endl;
    return NULL;
}
void Physics::Cleanup(){
    for(uint i = 0; i < (uint)physicsObjects.size(); i++)
        delete physicsObjects.at(i);
    physicsObjects.clear();
    nextID = 0;
    delete dynamicsWorld;
    delete solver;
    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
}
/*********************************************************/

Physics::Physics(String name, Transform transform, btScalar mass, std::string shapeType){
    this->name = name;
    if(shapeType == "sphere"){
        this->shape = new btSphereShape(transform.scale.x);
    }else if(shapeType == "box"){
        this->shape = new btBoxShape(btVector3(transform.scale.x, transform.scale.y, transform.scale.z));
    }else{
        std::cout << "Shape type: '" << shapeType << "'' is not a valid type!" << std::endl;
        std::cout << "Defaulting to sphere shape!" << std::endl;
        this->shape = new btSphereShape(1);
    }

    this->motionState = new btDefaultMotionState(
        btTransform(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w),
        btVector3(transform.position.x, transform.position.y, transform.position.z)));
    btVector3 inertia(0, 0, 0);
    this->shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo fallRigidbodyCI(mass, this->motionState, this->shape, inertia);
    this->rigidbody = new btRigidBody(fallRigidbodyCI);
    dynamicsWorld->addRigidBody(this->rigidbody);

    this->id = nextID;
    nextID++;
    physicsObjects.push_back(this);
}

Physics::~Physics(){
    dynamicsWorld->removeRigidBody(this->rigidbody);
    delete this->rigidbody->getMotionState();
    delete this->rigidbody;
    delete this->shape;
}
