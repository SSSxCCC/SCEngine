#include "sc/editor/EditorCameraController.h"
#include "sc/core/GameObject.h"
#include "sc/core/Scene.h"
#include "sc/core/Transform2D.h"
#include "sc/core/Engine.h"
#include <iostream>

namespace sc {

void EditorCameraController::onCreate() {
	mCamera = mGameObject->getScript<Camera>();
	mEditorInput = &mGameObject->mScene->mEngine->mInputManager->mEditorInput;
	if (!mCamera) throw std::runtime_error("EditorCameraController::onCreate Camera is not found");
	reset();
}

void EditorCameraController::zoomIn() {
	mCamera->mZoom /= 1.1f;
}
void EditorCameraController::zoomOut() {
	mCamera->mZoom *= 1.1f;
}

void EditorCameraController::reset() {
	auto transform2D = mGameObject->getScript<Transform2D>();
	if (transform2D) {
        transform2D->mPosition.x = 0.0f;
        transform2D->mPosition.y = 0.0f;
    }
	mCamera->mZoom = 1.0f / gScale;
}

void EditorCameraController::onUpdate() {
	// scroll to zoom
	float scrollX, scrollY;
	mEditorInput->getScroll(scrollX, scrollY);
	if (scrollY > 0.0f) {
		zoomIn();
	} else if (scrollY < 0.0f) {
		zoomOut();
	}

    // get transform2D
    auto transform2D = mGameObject->getScript<Transform2D>();
    if (!transform2D) {
        return;
    }

	// press home key to reset
	if (mEditorInput->getKey(KEY_HOME) == PRESS) {
		reset();
	}

	if (mEditorInput->getMouseButton(MOUSE_BUTTON_RIGHT) == PRESS) {
		// move by mouse drag
		float cursorX, cursorY;
		mEditorInput->getCursorPosition(cursorX, cursorY);
		if (mDragging) {
			float worldX, worldY;
			mCamera->screenToWorld(cursorX, cursorY, worldX, worldY);
            transform2D->mPosition.x += (mLastWorldX - worldX);
            transform2D->mPosition.y += (mLastWorldY - worldY);
		} else {
			mDragging = true;
		}
		mCamera->screenToWorld(cursorX, cursorY, mLastWorldX, mLastWorldY);
	} else {
		mDragging = false;

		// move by WASD or arrow keys
		float moveX = 0, moveY = 0;
		if (mEditorInput->getKey(KEY_LEFT) == PRESS || mEditorInput->getKey(KEY_A) == PRESS) {
			moveX -= 1.0f;
		}
		if (mEditorInput->getKey(KEY_RIGHT) == PRESS || mEditorInput->getKey(KEY_D) == PRESS) {
			moveX += 1.0f;
		}
		if (mEditorInput->getKey(KEY_UP) == PRESS || mEditorInput->getKey(KEY_W) == PRESS) {
			moveY += 1.0f;
		}
		if (mEditorInput->getKey(KEY_DOWN) == PRESS || mEditorInput->getKey(KEY_S) == PRESS) {
			moveY -= 1.0f;
		}
        transform2D->mPosition.x += moveX * mMoveSpeed * mGameObject->mScene->mDeltaTime;
        transform2D->mPosition.y += moveY * mMoveSpeed * mGameObject->mScene->mDeltaTime;
	}
}

void EditorCameraController::onDestroy() {
	mCamera = nullptr;
}

void EditorCameraController::setData(const ScriptData& data) {
	mMoveSpeed = data.get<float>("mMoveSpeed");
}

ScriptData EditorCameraController::getData() {
	auto data = Script::getData();
	data.add("mMoveSpeed", mMoveSpeed);
	return data;
}

} // namespace sc
