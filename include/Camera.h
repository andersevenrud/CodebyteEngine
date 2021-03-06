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

#ifndef CAMERA_H
#define CAMERA_H
#pragma once

#include "Tools.h"
#include "Transform.h"
#include "Shader.h"

class Camera{

// Static stuff
private:
    static ulong nextID;
    static std::vector<Camera*> cameras;
    static Transform*  currentCameraPos; // The position to render at
    static Camera*     currentCamera;    // The camera to render with
    static glm::mat4   projViewMat;
public:
    static Camera* At(ulong id);
    static Camera* At(String name);
    static void Cleanup();
    static glm::mat4* ProjViewMat();
    static void SetCurrentCamera(Camera* camera, Transform* transformPtr);
    static Camera* GetCurrentCamera();

// Instance stuff
private:
	bool projNeedsUpload;
	float fovy, aspect;
	void RecalcProjMatrix();
	void RecalcViewMatrix();
public:
    ulong id;
    String name;
    glm::mat4 projectionMatrix;
    Camera(String name, float fovy, float aspect);
	~Camera(void);
	void SetFovy(float value);
	void SetAspect(float value);
    glm::mat4 GetProjectionMatrix();
};

#endif /* CAMERA_H */
