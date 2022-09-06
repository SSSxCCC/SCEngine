#include "editor/EditorCameraController.h"
#include "input/Input.h"
#include "core/GameObject.h"
#include "core/GameWorld.h"
#include <iostream>

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
	// scroll to zoom
	float scrollX, scrollY;
	gEditorInput.getScroll(scrollX, scrollY);
	if (scrollY > 0.0f) {
		zoomIn();
	} else if (scrollY < 0.0f) {
		zoomOut();
	}

	// press home key to reset
	if (gEditorInput.getKey(GLFW_KEY_HOME) == GLFW_PRESS) {
		reset();
	}

	if (gEditorInput.getMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		// move by mouse drag
		float cursorX, cursorY;
		gEditorInput.getCursorPosition(cursorX, cursorY);
		float worldX, worldY;
		mCamera->screenToWorld(cursorX, cursorY, worldX, worldY);
		if (mDragging) {
			mGameObject->mTransform.mPosX += (mLastWorldX - worldX);
			mGameObject->mTransform.mPosY += (mLastWorldY - worldY);
		} else {
			mDragging = true;
		}
		mLastWorldX = worldX;
		mLastWorldY = worldY;
	} else {
		mDragging = false;

		// move by WASD or arrow keys
		float moveX = 0, moveY = 0;
		if (gEditorInput.getKey(GLFW_KEY_LEFT) == GLFW_PRESS || gEditorInput.getKey(GLFW_KEY_A) == GLFW_PRESS) {
			moveX -= 1.0f;
		}
		if (gEditorInput.getKey(GLFW_KEY_RIGHT) == GLFW_PRESS || gEditorInput.getKey(GLFW_KEY_D) == GLFW_PRESS) {
			moveX += 1.0f;
		}
		if (gEditorInput.getKey(GLFW_KEY_UP) == GLFW_PRESS || gEditorInput.getKey(GLFW_KEY_W) == GLFW_PRESS) {
			moveY += 1.0f;
		}
		if (gEditorInput.getKey(GLFW_KEY_DOWN) == GLFW_PRESS || gEditorInput.getKey(GLFW_KEY_S) == GLFW_PRESS) {
			moveY -= 1.0f;
		}
		mGameObject->mTransform.mPosX += moveX * mMoveSpeed * mGameObject->mGameWorld->mDeltaTime;
		mGameObject->mTransform.mPosY += moveY * mMoveSpeed * mGameObject->mGameWorld->mDeltaTime;
	}
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
