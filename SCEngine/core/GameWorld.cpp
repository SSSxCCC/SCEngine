#include "core/GameWorld.h"

void GameWorld::create() {
	mDebugDraw.Create(shared_from_this());
	mPhysicsWorld.SetDebugDraw(&mDebugDraw);
	for (auto gameObject : mGameObjects) {
		gameObject->onCreate();
	}
	mCreated = true;
}

void GameWorld::update() {
	// First update physics world
	mPhysicsWorld.Step(mDeltaTime, velocityIterations, positionIterations);

	// Then update game objects
	for (auto gameObject : mGameObjects) {
		gameObject->onUpdate();
	}

	// Last debug draw the physics objects
	mDebugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit);
	mPhysicsWorld.DebugDraw();
	mDebugDraw.Flush();
}

void GameWorld::destroy() {
	mDebugDraw.Destroy();
	// TODO: destroy all gameObjects
}

void GameWorld::addGameObject(const std::shared_ptr<GameObject>& gameObject) {
	mGameObjects.push_back(gameObject);
	gameObject->mGameWorld = shared_from_this();
	if (mCreated) {
		gameObject->onCreate();
	}
}
