#include "SCEngine.h"

#include <iostream>

#include "box2d/box2d.h"

#include "utility/Window.h"
#include "utility/Asset.h"
#include "input/Input.h"
#include "core/GameWorld.h"
#include "core/Transform2D.h"
#include "graphics/RectangleRender.h"
#include "physics2d/RigidBody2D.h"
#include "physics2d/RectangleCollider2D.h"
#include "editor/EditorCameraController.h"
#include "editor/DebugDrawPhysics2D.h"

std::shared_ptr<GameWorld> gameWorld;
GameWorldData gameWorldData, tempGameWorldData;
std::chrono::steady_clock::time_point startTime;

void createEmptyGame() {
    if (gameWorld) {
        gameWorld->destroy();
    }

    // Add some GameObject to game world
    gameWorld = std::make_shared<GameWorld>();

    auto physicsObject = std::make_shared<GameObject>("PhysicsWorld2D");
    physicsObject->addScript(std::make_shared<PhysicsWorld2D>());
    physicsObject->addScript(std::make_shared<DebugDrawPhysics2D>());
    gameWorld->addGameObject(physicsObject);

    auto cameraObject = std::make_shared<GameObject>("EditorCamera");
    cameraObject->addScript(std::make_shared<Transform2D>());
    cameraObject->addScript(std::make_shared<Camera>());
    cameraObject->addScript(std::make_shared<EditorCameraController>());
    gameWorld->addGameObject(cameraObject);

    auto camera2Object = std::make_shared<GameObject>("GameCamera");
    camera2Object->addScript(std::make_shared<Transform2D>());
    camera2Object->addScript(std::make_shared<Camera>());
    gameWorld->addGameObject(camera2Object);

    auto gameObject = std::make_shared<GameObject>("Box1");
    gameObject->addScript(std::make_shared<Transform2D>());
    auto transform2D = gameObject->getScript<Transform2D>();
    transform2D->mScale.x = 10.f;
    transform2D->mScale.y = 10.f;
    transform2D->mPosition.x = 0.0f;
    transform2D->mPosition.y = 100.0f;
    gameObject->addScript(std::make_shared<RectangleRender>());
    gameObject->addScript(std::make_shared<RigidBody2D>());
    gameObject->getScript<RigidBody2D>()->mBodyDef.type = b2_dynamicBody;
    gameObject->addScript(std::make_shared<RectangleCollider2D>());
    gameWorld->addGameObject(gameObject);

    auto gameObject2 = gameObject->clone();
    gameObject2->mName = "Box2";
    transform2D = gameObject2->getScript<Transform2D>();
    transform2D->mPosition.x = 5.0f;
    transform2D->mPosition.y = 120.0f;
    gameWorld->addGameObject(gameObject2);

    auto groundObject = gameObject->clone();
    groundObject->mName = "Ground";
    groundObject->getScript<RigidBody2D>()->mBodyDef.type = b2_staticBody;
    transform2D = groundObject->getScript<Transform2D>();
    transform2D->mPosition.x = 0.0f;
    transform2D->mPosition.y = -100.0f;
    transform2D->mScale.x = 400.0f;
    transform2D->mScale.y = 80.f;
    gameWorld->addGameObject(groundObject);

    gameWorld->create();
    startTime = std::chrono::steady_clock::now();
    gameWorldData = gameWorld->getData();
}

void reloadGame() {
    if (gameWorld) {
        gameWorld->destroy();
    }
    gameWorld = GameWorld::create(gameWorldData);
    gameWorld->create();
    startTime = std::chrono::steady_clock::now();
}

void init(CallbackPointer& callbackPointer, const fs::path& assetDir) {
	callbackPointer.mScrollCallback = [](double dx, double dy) {
		gEditorInput.setScroll((float)dx, (float)dy);
		gInput.setScroll((float)dx, (float)dy);
	};
    Input::sGetKey = callbackPointer.mGetKey;
    Input::sGetMouseButton = callbackPointer.mGetMouseButton;
    Input::sGetCursorPos = callbackPointer.mGetCursorPos;
    gAssetDir = assetDir;

    std::cout << "Box2D Version " << b2_version.major << "." << b2_version.minor << "." << b2_version.revision << std::endl;
}

GameWorldData& doFrame(bool editorMode) {
	if (editorMode) {
		//reloadGame();
		//std::cout << "editType=" << static_cast<typename std::underlying_type<EditType>::type>(gameWorldData.editType) << std::endl;
		if (gameWorldData.editType == EditType::Modify) {
			gameWorld->setData(gameWorldData);
		} // TODO: hanle other edit type
	} else {
		if (tempGameWorldData.editType == EditType::Modify) {
			gameWorld->setData(tempGameWorldData);
		} // TODO: hanle other edit type
	}

	float currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count() / 1000000000.0f;
	gameWorld->mDeltaTime = currentTime - gameWorld->mCurrentTime;
	gameWorld->mCurrentTime = currentTime;
	//std::cout << "editorMode=" << editorMode << ", currentTime=" << gameWorld->mCurrentTime << ", deltaTime=" << gameWorld->mDeltaTime << std::endl;

	if (editorMode) {
		gameWorld->mEditorCamera->mGameObject->update(); // only update EditorCamera in editor mode
	} else {
		gameWorld->update();
	}

	if (editorMode) {
		gameWorldData = gameWorld->getData();
		return gameWorldData;
	} else {
		tempGameWorldData = gameWorld->getData();
		return tempGameWorldData;
	}
}

void doEditorFrame(bool focus, int with, int height, float cursorOffsetX, float cursorOffsetY) {
	gEditorInput.setFocus(focus);
	gEditorInput.setCursorOffset(cursorOffsetX, cursorOffsetY);
	gameWorld->mEditorCamera->setSize(with, height);
	gameWorld->draw(true);
	gEditorInput.reset();
}

void doGameFrame(bool focus, int with, int height, float cursorOffsetX, float cursorOffsetY) {
	gInput.setFocus(focus);
	gInput.setCursorOffset(cursorOffsetX, cursorOffsetY);
	gameWorld->mMainCamera->setSize(with, height);
	gameWorld->draw();
	gInput.reset();
}

void runGame() {
	gameWorldData = gameWorld->getData();
	reloadGame();
}

void stopGame() {
	reloadGame();
}

nlohmann::json save() {
    return gameWorld->getData();
}

void load(const nlohmann::json& j) {
    if (j.empty()) {
        createEmptyGame();
    } else {
        gameWorldData = j;
        reloadGame();
    }
}

void close() {
	gameWorld->destroy();
	gameWorld = nullptr;
}
