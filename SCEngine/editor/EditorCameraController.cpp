#include "editor/EditorCameraController.h"
#include "core/Input.h"
#include "core/GameObject.h"
#include "core/GameWorld.h"

void EditorCameraController::onCreate() {
	mCamera = mGameObject->getScript<Camera>();
	assert(mCamera);
	reset();
}

void EditorCameraController::zoomIn() {
	mCamera->mZoom /= 1.1f;
}
void EditorCameraController::zoomOut() {
	mCamera->mZoom *= 1.1f;
}

void EditorCameraController::reset() {
	mGameObject->mTransform.mPosX = 0.0f;
	mGameObject->mTransform.mPosY = 0.0f;
	mCamera->mZoom = 1.0f;
}

void EditorCameraController::onUpdate() {
	if (gInput.mScrollY > 0.0f) {
		zoomIn();
	}
	else if (gInput.mScrollY < 0.0f) {
		zoomOut();
	}

	if (gInput.getKey(GLFW_KEY_HOME) == GLFW_PRESS) {
		reset();
	}

	float moveX = 0, moveY = 0;
	if (gInput.getKey(GLFW_KEY_LEFT) == GLFW_PRESS || gInput.getKey(GLFW_KEY_A) == GLFW_PRESS) {
		moveX -= 1.0f;
	}
	if (gInput.getKey(GLFW_KEY_RIGHT) == GLFW_PRESS || gInput.getKey(GLFW_KEY_D) == GLFW_PRESS) {
		moveX += 1.0f;
	}
	if (gInput.getKey(GLFW_KEY_UP) == GLFW_PRESS || gInput.getKey(GLFW_KEY_W) == GLFW_PRESS) {
		moveY += 1.0f;
	}
	if (gInput.getKey(GLFW_KEY_DOWN) == GLFW_PRESS || gInput.getKey(GLFW_KEY_S) == GLFW_PRESS) {
		moveY -= 1.0f;
	}
	mGameObject->mTransform.mPosX += moveX * mMoveSpeed * mGameObject->mGameWorld->mDeltaTime;
	mGameObject->mTransform.mPosY += moveY * mMoveSpeed * mGameObject->mGameWorld->mDeltaTime;
}

void EditorCameraController::onDestroy() {
	mCamera = nullptr;
}

void EditorCameraController::setData(const ScriptData& data) {
	 mMoveSpeed = data.getFloat("mMoveSpeed");
}

ScriptData EditorCameraController::getData() {
	auto data = Script::getData();
	data.addFloat("mMoveSpeed", mMoveSpeed);
	return data;
}
