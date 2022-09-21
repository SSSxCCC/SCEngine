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

	auto physicsObject = std::make_shared<GameObject>("PhysicsWorld");
	physicsObject->addScript(std::make_shared<PhysicsWorld>());
	physicsObject->addScript(std::make_shared<PhysicsDebugDraw>());
	gameWorld->addGameObject(physicsObject);

	auto cameraObject = std::make_shared<GameObject>("EditorCamera");
	cameraObject->addScript(std::make_shared<Transform>());
	cameraObject->addScript(std::make_shared<Camera>());
	cameraObject->addScript(std::make_shared<EditorCameraController>());
	gameWorld->addGameObject(cameraObject);

	auto camera2Object = std::make_shared<GameObject>("GameCamera");
	camera2Object->addScript(std::make_shared<Transform>());
	camera2Object->addScript(std::make_shared<Camera>());
	gameWorld->addGameObject(camera2Object);

	auto gameObject = std::make_shared<GameObject>("Box1");
	gameObject->addScript(std::make_shared<Transform>());
	auto transform = gameObject->getScript<Transform>();
	transform->mScaleX = 10.f;
	transform->mScaleY = 10.f;
	transform->mPosX = 0.0f;
	transform->mPosY = 100.0f;
	gameObject->addScript(std::make_shared<RectangleRender>());
	gameObject->addScript(std::make_shared<RigidBody>());
	gameObject->getScript<RigidBody>()->mBodyDef.type = b2_dynamicBody;
	gameObject->addScript(std::make_shared<RectangleCollider>());
	gameWorld->addGameObject(gameObject);

	auto gameObject2 = gameObject->clone();
	gameObject2->mName = "Box2";
	transform = gameObject2->getScript<Transform>();
	transform->mPosX = 5.0f;
	transform->mPosY = 120.0f;
	gameWorld->addGameObject(gameObject2);

	auto groundObject = gameObject->clone();
	groundObject->mName = "Ground";
	groundObject->getScript<RigidBody>()->mBodyDef.type = b2_staticBody;
	transform = groundObject->getScript<Transform>();
	transform->mPosX = 0.0f;
	transform->mPosY = -100.0f;
	transform->mScaleX = 400.0f;
	transform->mScaleY = 80.f;
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
