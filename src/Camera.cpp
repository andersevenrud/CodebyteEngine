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

#include "Camera.h"

/********************** Static code **********************/
ulong Camera::nextID = 0;
std::vector<Camera*> Camera::cameras;
Transform* Camera::currentCameraPos = NULL;
Camera* Camera::currentCamera = NULL;
glm::mat4 Camera::projViewMat;
Camera* Camera::At(ulong id){
    for(uint i = 0; i < (uint)cameras.size(); i++){
        if(cameras.at(i)->id == id) return cameras.at(i);
    }
    return NULL;
}
Camera* Camera::At(String name){
    for(uint i = 0; i < (uint)cameras.size(); i++){
        if(cameras.at(i)->name == name) return cameras.at(i);
    }
    std::cout << "Camera with name: " << name << " does not exist!" << std::endl;
    return NULL;
}
void Camera::Cleanup(){
    for(uint i = 0; i < (uint)cameras.size(); i++)
        delete cameras.at(i);
    cameras.clear();
    nextID = 0;
}

glm::mat4* Camera::ProjViewMat(){
    if(currentCamera != NULL && currentCameraPos != NULL){
        projViewMat = currentCamera->projectionMatrix * glm::inverse(currentCameraPos->globalModelMatrix);
        return &projViewMat;
    }else{
        std::cout << "ProjViewMat: Current camera can not be NULL!" << std::endl;
        return NULL;
    }
}

void Camera::SetCurrentCamera(Camera* camera, Transform* transformPtr){
    if(transformPtr != NULL) currentCameraPos = transformPtr;
    if(camera != NULL) currentCamera = camera;
}

Camera* Camera::GetCurrentCamera(){
    return currentCamera;
}

/*********************************************************/

Camera::Camera(String name, float fovy, float aspect){
    this->name = name;
    this->projNeedsUpload = false;
	this->fovy = fovy;
	this->aspect = aspect;
	this->RecalcProjMatrix();

    this->id = nextID;
    nextID++;
    cameras.push_back(this);
}

Camera::~Camera(void){

}

void Camera::RecalcProjMatrix(){
	this->projectionMatrix = glm::perspective(this->fovy, this->aspect, 0.1f, 1000.0f);
}

void Camera::SetFovy(float value){
	this->fovy = value;
	this->RecalcProjMatrix();
}

void Camera::SetAspect(float value){
	this->aspect = value;
	this->RecalcProjMatrix();
}

glm::mat4 Camera::GetProjectionMatrix(){
	return this->projectionMatrix;
}
