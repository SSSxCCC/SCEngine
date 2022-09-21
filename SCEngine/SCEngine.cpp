#include "SCEngine.h"

#include <iostream>

#include "box2d/box2d.h"

#include "utility/Window.h"
#include "input/Input.h"
#include "core/GameWorld.h"
#include "graphics/RectangleRender.h"
#include "physics/RigidBody.h"
#include "physics/RectangleCollider.h"
#include "editor/EditorCameraController.h"
#include "editor/PhysicsDebugDraw.h"

std::shared_ptr<GameWorld> gameWorld;
nlohmann::json j;
std::chrono::steady_clock::time_point startTime;

void init(OpenGLPointer& openGLPointer, CallbackPointer& callbackPointer) {
	openGLPointer.apply();
	callbackPointer.mScrollCallback = [](double dx, double dy) {
		gEditorInput.setScroll((float)dx, (float)dy);
		gInput.setScroll((float)dx, (float)dy);
	};

	printf("Box2D Version %d.%d.%d\n", b2_version.major, b2_version.minor, b2_version.revision);

	// Add some GameObject to game world
	gameWorld = std::make_shared<GameWorld>();

	auto physicsObject = std::make_shared<GameObject>();
	physicsObject->mName = "PhysicsWorld";
	auto physicsWorld = std::make_shared<PhysicsWorld>();
	auto debugDraw = std::make_shared<PhysicsDebugDraw>();
	physicsObject->addScript(physicsWorld);
	physicsObject->addScript(debugDraw);
	gameWorld->addGameObject(physicsObject);

	auto cameraObject = std::make_shared<GameObject>();
	cameraObject->mName = "EditorCamera";
	auto camera = std::make_shared<Camera>();
	auto editorCameraController = std::make_shared<EditorCameraController>();
	cameraObject->addScript(camera);
	cameraObject->addScript(editorCameraController);
	gameWorld->addGameObject(cameraObject);

	auto camera2Object = std::make_shared<GameObject>();
	camera2Object->mName = "GameCamera";
	auto camera2 = std::make_shared<Camera>();
	camera2Object->addScript(camera2);
	gameWorld->addGameObject(camera2Object);

	auto rectangleRender = std::make_shared<RectangleRender>();
	auto rigidBody = std::make_shared<RigidBody>();
	rigidBody->mBodyDef.type = b2_dynamicBody;
	auto rectangleCollider = std::make_shared<RectangleCollider>();
	auto gameObject = std::make_shared<GameObject>();
	gameObject->mName = "Box1";
	gameObject->mTransform.mScaleX = 10.f;
	gameObject->mTransform.mScaleY = 10.f;
	gameObject->mTransform.mPosX = 0.0f;
	gameObject->mTransform.mPosY = 100.0f;
	gameObject->addScript(rectangleRender);
	gameObject->addScript(rigidBody);
	gameObject->addScript(rectangleCollider);
	gameWorld->addGameObject(gameObject);

	auto gameObject2 = gameObject->clone();
	gameObject2->mName = "Box2";
	gameObject2->mTransform.mPosX = 5.0f;
	gameObject2->mTransform.mPosY = 120.0f;
	gameWorld->addGameObject(gameObject2);

	auto groundObject = gameObject->clone();
	groundObject->mName = "Ground";
	groundObject->getScript<RigidBody>()->mBodyDef.type = b2_staticBody;
	groundObject->mTransform.mPosX = 0.0f;
	groundObject->mTransform.mPosY = -100.0f;
	groundObject->mTransform.mScaleX = 400.0f;
	groundObject->mTransform.mScaleY = 80.f;
	gameWorld->addGameObject(groundObject);

	gameWorld->create();

	j = gameWorld->getData();
	std::cout << j << std::endl;

	startTime = std::chrono::steady_clock::now();
}

void doFrame(bool editorMode) {
	float currentTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - startTime).count() / 1000000000.0f;
	gameWorld->mDeltaTime = currentTime - gameWorld->mCurrentTime;
	gameWorld->mCurrentTime = currentTime;
	//std::cout << "editorMode=" << editorMode << ", currentTime=" << gameWorld->mCurrentTime << ", deltaTime=" << gameWorld->mDeltaTime << std::endl;

	if (editorMode) {
		gameWorld->mEditorCamera->mGameObject->update(); // only update EditorCamera in editor mode
	} else {
		gameWorld->update();
	}
}

void doEditorFrame(bool focus, int with, int height) {
	gEditorInput.setFocus(focus);
	//gEditorInput.setCursorOffset(cursorOffsetX, cursorOffsetY);
	gameWorld->mEditorCamera->setSize(with, height);
	gameWorld->draw(true);
	gEditorInput.reset();
}

void doGameFrame(bool focus, int with, int height) {
	gInput.setFocus(focus);
	//gInput.setCursorOffset(cursorScreenPos.x, cursorScreenPos.y);
	gameWorld->mMainCamera->setSize(with, height);
	gameWorld->draw();
	gInput.reset();
}

static auto reloadGame = [&]() {
	gameWorld->destroy();
	GameWorldData gameWorldData = j;
	gameWorld = GameWorld::create(gameWorldData);
	gameWorld->create();
};

void runGame() {
	j = gameWorld->getData();
	reloadGame();
}

void stopGame() {
	reloadGame();
}

void save() {
	j = gameWorld->getData();
	std::ofstream o("GameWorldData.json");
	o << std::setw(4) << j << std::endl;
}

void load() {
	std::ifstream i("GameWorldData.json");
	if (i.good()) {
		i >> j;
		reloadGame();
	} else {
		std::cout << "Load error: failed to open GameWorldData.json" << std::endl;
	}
}

void close() {
	gameWorld->destroy();
	gameWorld = nullptr;
}
