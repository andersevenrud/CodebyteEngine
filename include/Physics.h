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

#ifndef PHYSICS_H
#define PHYSICS_H
#pragma once

#include "Tools.h"
#include "Transform.h"

class Physics{
// Static stuff
private:
    static float timeStep;
    static btBroadphaseInterface*                 broadphase;
    static btDefaultCollisionConfiguration*       collisionConfiguration;
    static btCollisionDispatcher*                 dispatcher;
    static btSequentialImpulseConstraintSolver*   solver;
    static btDiscreteDynamicsWorld*               dynamicsWorld;
    static ulong nextID;
    static std::vector<Physics*> physicsObjects;
public:
    static void Setup(glm::vec3 gravity, float fixedTimeStep);
    static void Update();
    static Physics* At(ulong id);
    static Physics* At(String name);
    static void Cleanup();

// Instance Stuff
public:
    ulong id;
    String name;
    btRigidBody* rigidbody;
    btCollisionShape* shape;
    btDefaultMotionState* motionState;
    Physics(String name, Transform transform, btScalar mass, std::string shapeType);
    ~Physics();
};


#endif /* PHYSICS_H */
