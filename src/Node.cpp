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

#include "Node.h"

/********************** Static code **********************/
ulong Node::nextID = 0;
std::vector<Node*> Node::nodes;
bool Node::sortByShaderId(Node* firstNode, Node* secondNode){
    if(firstNode->materialPtr != NULL && secondNode->materialPtr != NULL)
        return (firstNode->materialPtr->shader->shaderID < secondNode->materialPtr->shader->shaderID);
    else
        return false;
}
Node* Node::At(ulong id){
    for(uint i = 0; i < (uint)nodes.size(); i++){
        if(nodes.at(i)->id == id) return nodes.at(i);
    }
    std::cout << "Node with id: '" << id << "' does not exist!" << std::endl;
    return NULL;
}
Node* Node::At(String name){
    for(uint i = 0; i < (uint)nodes.size(); i++){
        if(nodes.at(i)->name == name) return nodes.at(i);
    }
    std::cout << "Node with name: '" << name << "' does not exist!" << std::endl;
    return NULL;
}
void Node::Cleanup(){
    for(uint i = 0; i < (uint)nodes.size(); i++)
        delete nodes.at(i);
    nodes.clear();
    nextID = 0;
}
void Node::Sort(){
    std::cout << "--------------------SORTING NODES--------------------" << std::endl;

    // Old node order
    for(uint i = 0; i < (uint)nodes.size(); i++){
        if(nodes.at(i)->materialPtr != NULL)
            std::cout << nodes.at(i)->materialPtr->shader->shaderID << " ";
        else
            std::cout << "0 ";
    }std::cout << std::endl;

    std::sort(nodes.begin(), nodes.end(), sortByShaderId);

    // New node order
    for(uint i = 0; i < (uint)nodes.size(); i++){
        if(nodes.at(i)->materialPtr != NULL)
            std::cout << nodes.at(i)->materialPtr->shader->shaderID << " ";
        else
            std::cout << "0 ";
    }std::cout << std::endl;
}
/*********************************************************/

/* Default constructor, simply assigns its id */
Node::Node(String name){
    this->name = name;
	this->parentNode = NULL;
	this->meshPtr = NULL;
	this->materialPtr = NULL;
    this->cameraPtr = NULL;
    this->physicsPtr = NULL;
    this->transform = Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::quat(0.0f, 0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    this->id = nextID;
    nextID++;
    nodes.push_back(this);
}

/* Second constructor, assigns its id, its transform, its mesh, and its material */
Node::Node(String name, Transform transform){
    this->name = name;
	this->parentNode = NULL;
    this->meshPtr = NULL;
    this->materialPtr = NULL;
    this->cameraPtr = NULL;
    this->physicsPtr = NULL;
	this->transform = transform;

    this->id = nextID;
    nextID++;
    nodes.push_back(this);
}

/* Invalidate the mesh and material pointers */
Node::~Node(void){
	this->meshPtr = NULL;
	this->materialPtr = NULL;
}

void Node::SetMesh(Mesh * object){
    this->meshPtr = object;
}

void Node::SetMaterial(Material * object){
    this->materialPtr = object;
}

void Node::SetCamera(Camera * object){
    this->cameraPtr = object;
}

void Node::SetPhysics(Physics * object){
    this->physicsPtr = object;
}


void Node::SetCameraToCurrent(){
    if(this->cameraPtr != NULL)
        Camera::SetCurrentCamera(this->cameraPtr, &this->transform);
    else
        std::cout << "cameraPtr must not be NULL to call SetCameraToCurrent!" << std::endl;
}

void Node::Translate(float x, float y, float z, bool global){
    if(global == true) this->transform.TranslateGlobal(x, y, z);
    else this->transform.TranslateLocal(x, y, z);
    /* Syncs the physics body with our new transform */
    if(this->physicsPtr != NULL){
        if(!this->physicsPtr->rigidbody->isActive()) this->physicsPtr->rigidbody->activate(true);
        SyncTransformAndRigidbody(true, 1);
    }
}

void Node::Rotate(float x, float y, float z, bool global){
    if(global == true) this->transform.RotateGlobal(x, y, z);
    else this->transform.RotateLocal(x, y, z);
     /* Syncs the physics body with our new transform */
    if(this->physicsPtr != NULL){
        if(!this->physicsPtr->rigidbody->isActive()) this->physicsPtr->rigidbody->activate(true);
        SyncTransformAndRigidbody(true, 2);
    }
}

void Node::Scale(float x, float y, float z, bool global){
    if(global == true) this->transform.ScaleGlobal(x, y, z);
    else this->transform.ScaleLocal(x, y, z);
}

void Node::ApplyForce(btVector3 force, bool global){
    if(this->physicsPtr != NULL){
        if(!this->physicsPtr->rigidbody->isActive()) this->physicsPtr->rigidbody->activate(true);

        if(global == true){
            this->physicsPtr->rigidbody->applyCentralForce(force);
        }else{
            btMatrix3x3& rot = this->physicsPtr->rigidbody->getWorldTransform().getBasis();
            btVector3 localForce = rot * force;
            this->physicsPtr->rigidbody->applyCentralForce(localForce);
        }
    }else{
        std::cout << "This object has no physics component attached!" << std::endl;
    }
}

void Node::ApplyImpulse(btVector3 impulse, bool global){

}

void Node::ApplyTorque(btVector3 torque){

}

/* for partsToSync allows for updating certain things only
 * 0 = both, 1 = position only, 2 = rotation only */
void Node::SyncTransformAndRigidbody(bool transOrRig, uint partsToSync){
    if(this->physicsPtr != NULL){
        if(transOrRig == false){
            btTransform trans;
            this->physicsPtr->motionState->getWorldTransform(trans);
            this->transform.position.x = trans.getOrigin().getX();
            this->transform.position.y = trans.getOrigin().getY();
            this->transform.position.z = trans.getOrigin().getZ();
            this->transform.rotation.x = trans.getRotation().getX();
            this->transform.rotation.y = trans.getRotation().getY();
            this->transform.rotation.z = trans.getRotation().getZ();
            this->transform.rotation.w = trans.getRotation().getW();
        }else{
            btTransform transformation;
            if(partsToSync == 0){
                transformation = btTransform(btQuaternion(this->transform.rotation.x, this->transform.rotation.y, this->transform.rotation.z, this->transform.rotation.w),
                    btVector3(this->transform.position.x, this->transform.position.y, this->transform.position.z));
            }else if(partsToSync == 1){
                transformation = btTransform(this->physicsPtr->rigidbody->getWorldTransform().getRotation(),
                    btVector3(this->transform.position.x, this->transform.position.y, this->transform.position.z));
            }else{
                transformation = btTransform(btQuaternion(this->transform.rotation.x, this->transform.rotation.y, this->transform.rotation.z, this->transform.rotation.w),
                    this->physicsPtr->rigidbody->getWorldTransform().getOrigin());
            }
            this->physicsPtr->rigidbody->setWorldTransform(transformation);
        }
    }else{
        std::cout << "This object has no physics component attached!" << std::endl;
    }
}

/* Sets this objects parent, remembering to tell its 
current parent to remove is from its childs list */
void Node::SetParent(Node* node){
    if(this->parentNode != NULL){
		this->parentNode->RemoveChild(this->id);
    }
    this->parentNode = node;
}

Node* Node::GetParent(){
    if(this->parentNode != NULL){
        return this->parentNode;
    }else{
        return NULL;
    }
}

/* Adds a childNode */
void Node::AddChild(Node* node){
	this->childNodes.push_back(node);
	node->SetParent(this);
}

/* Removes a childNode */
void Node::RemoveChild(ulong id){
	for(uint i = 0; i < (uint)this->childNodes.size(); i++){
		if(this->childNodes.at(i)->id == id){
			this->childNodes.erase(this->childNodes.begin() + i);
			return;
		}
	}
}

void Node::Update(glm::mat4* combinedModelMatrix){
    if(this->physicsPtr != NULL){
        this->SyncTransformAndRigidbody(false, 0);
    }

    *combinedModelMatrix *= this->transform.calcLocalModelMatrix();
    this->transform.globalModelMatrix = *combinedModelMatrix;

    glm::mat4 temp = *combinedModelMatrix;

    for(uint i = 0; i < (uint)this->childNodes.size(); i++){
        temp = *combinedModelMatrix;
        this->childNodes.at(i)->Update(combinedModelMatrix);
        *combinedModelMatrix = temp;
    }
}

void Node::Render(glm::mat4* projectionViewMatrix){
    // Can we render it?
    if(this->meshPtr != NULL && this->materialPtr != NULL){
        glm::mat4 final(*projectionViewMatrix * this->transform.globalModelMatrix);
        glUniformMatrix4fv(this->materialPtr->shader->GetUniform("MVP"), 1, GL_FALSE, &final[0][0]);
        this->meshPtr->Render();
    }
}
