#include "core/Camera.h"
#include "core/GameObject.h"
#include "core/Scene.h"
#include "core/Transform2D.h"
#include "editor/EditorCameraController.h"

void Camera::onCreate() {
	auto editorCameraController = mGameObject->getScript<EditorCameraController>();
	if (editorCameraController) {
		mEditor = true;
		mGameObject->mScene->mEditorCamera = std::dynamic_pointer_cast<Camera>(shared_from_this());
	} else {
		mEditor = false;
		mGameObject->mScene->mMainCamera = std::dynamic_pointer_cast<Camera>(shared_from_this());
	}
}

void Camera::onDestroy() {
	if (mEditor) {
		if (mGameObject->mScene->mEditorCamera == shared_from_this()) {
			mGameObject->mScene->mEditorCamera = nullptr;
		}
	} else {
		if (mGameObject->mScene->mMainCamera == shared_from_this()) {
			mGameObject->mScene->mMainCamera = nullptr;
		}
	}
}

glm::mat4 Camera::buildProjectionMatrix() {
	auto transform2D = mGameObject->getScript<Transform2D>();
	glm::vec3 cameraPos = transform2D ? transform2D->mPosition : glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 projection = glm::ortho((float)-mHalfWidth * mZoom, (float)mHalfWidth * mZoom, (float)-mHalfHeight * mZoom, (float)mHalfHeight * mZoom, -1000.0f, 1000.0f);
	projection[1][1] *= -1;  // GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted. The easiest way to compensate for that is to flip the sign on the scaling factor of the Y axis in the projection matrix.
	return projection * view;
}

void Camera::worldToScreen(float worldX, float worldY, float& screenX, float& screenY) {
    auto transform2D = mGameObject->getScript<Transform2D>();
    float left = (transform2D ? transform2D->mPosition.x : 0.0f) - (float)mHalfWidth * mZoom;
    float top = (transform2D ? transform2D->mPosition.y : 0.0f) + (float)mHalfHeight * mZoom;
    screenX = (worldX - left) / mZoom;
    screenY = (top - worldY) / mZoom;
}

void Camera::screenToWorld(float screenX, float screenY, float& worldX, float& worldY) {
    auto transform2D = mGameObject->getScript<Transform2D>();
    float left = (transform2D ? transform2D->mPosition.x : 0.0f) - (float)mHalfWidth * mZoom;
    float top = (transform2D ? transform2D->mPosition.y : 0.0f) + (float)mHalfHeight * mZoom;
    worldX = left + screenX * mZoom;
	worldY = top - screenY * mZoom;
}

void Camera::setData(const ScriptData& data) {
	mHalfWidth = data.get<int>("mHalfWidth");
	mHalfHeight = data.get<int>("mHalfHeight");
	mZoom = data.get<float>("mZoom");
}

ScriptData Camera::getData() {
	auto data = Script::getData();
	data.add("mHalfWidth", mHalfWidth);
	data.add("mHalfHeight", mHalfHeight);
	data.add("mZoom", mZoom);
	return data;
}
