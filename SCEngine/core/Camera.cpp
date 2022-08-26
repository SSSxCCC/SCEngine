#include "core/Camera.h"
#include "core/GameObject.h"
#include "core/GameWorld.h"

void Camera::onCreate() {
	mGameObject->mGameWorld->mMainCamera = std::dynamic_pointer_cast<Camera>(shared_from_this());
}

void Camera::onDestroy() {
	if (mGameObject->mGameWorld->mMainCamera == shared_from_this()) {
		mGameObject->mGameWorld->mMainCamera = nullptr;
	}
}

glm::mat4 Camera::buildProjectionMatrix() {
	glm::vec3 cameraPos = glm::vec3(mGameObject->mTransform.mPosX, mGameObject->mTransform.mPosY, mGameObject->mTransform.mZ);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 projection = glm::ortho((float)-mHalfWidth * mZoom, (float)mHalfWidth * mZoom, (float)-mHalfHeight * mZoom, (float)mHalfHeight * mZoom, -1000.0f, 1000.0f);
	return projection * view;
}

void Camera::worldToScreen(float worldX, float worldY, float& screenX, float& screenY) {
	float left = mGameObject->mTransform.mPosX - (float)mHalfWidth * mZoom;
	float top = mGameObject->mTransform.mPosY + (float)mHalfHeight * mZoom;
	screenX = (worldX - left) / mZoom;
	screenY = (top - worldY) / mZoom;
}

void Camera::screenToWorld(float screenX, float screenY, float& worldX, float& worldY) {
	float left = mGameObject->mTransform.mPosX - (float)mHalfWidth * mZoom;
	float top = mGameObject->mTransform.mPosY + (float)mHalfHeight * mZoom;
	worldX = left + screenX * mZoom;
	worldY = top - screenY * mZoom;
}

void Camera::setData(const ScriptData& data) {
	mHalfWidth = data.intData.at("mHalfWidth");
	mHalfHeight = data.intData.at("mHalfHeight");
	mZoom = data.floatData.at("mZoom");
}

ScriptData Camera::getData() {
	auto data = Script::getData();
	data.intData["mHalfWidth"] = mHalfWidth;
	data.intData["mHalfHeight"] = mHalfHeight;
	data.floatData["mZoom"] = mZoom;
	return std::move(data);
}
