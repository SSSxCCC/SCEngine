#include "core/GameWorld.h"
#include <iostream>

void GameWorld::create() {
	mDebugDraw.Create(shared_from_this());
	mPhysicsWorld.SetDebugDraw(&mDebugDraw);
	for (const auto& [_, gameObject] : mGameObjects) {
		gameObject->onCreate();
	}
	mCreated = true;
}

void GameWorld::update() {
	// First update physics world
	mPhysicsWorld.Step(mDeltaTime, velocityIterations, positionIterations);

	// Then update game objects
	for (const auto& [_, gameObject] : mGameObjects) {
		gameObject->onUpdate();
	}

	// Last debug draw the physics objects
	mDebugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit);
	mPhysicsWorld.DebugDraw();
	mDebugDraw.Flush();
}

void GameWorld::destroy() {
	mDebugDraw.Destroy();

	while (mGameObjectIds.size() > 0) {
		removeGameObject(mGameObjectIds[mGameObjectIds.size() - 1]);
	}
	assert(mGameObjectIds.size() == 0);
	assert(mGameObjects.size() == 0);
}

void GameWorld::addGameObject(const std::shared_ptr<GameObject>& gameObject) {
	gameObject->mId = generateId();
	mGameObjectIds.push_back(gameObject->mId);
	mGameObjects[gameObject->mId] = gameObject;
	gameObject->mGameWorld = shared_from_this();
	if (mCreated) {
		gameObject->onCreate();
	}
}

void GameWorld::removeGameObject(int gameObjectId) {
	if (!mGameObjects.contains(gameObjectId)) {
		std::cout << "GameWorld::removeGameObject GameObjectId is not exist, GameObjectId=" << gameObjectId << std::endl;
		return;
	}
	mGameObjects[gameObjectId]->onDestroy();
	mGameObjects[gameObjectId]->mGameWorld = nullptr;
	mGameObjects.erase(gameObjectId);
	auto iter = std::remove(mGameObjectIds.begin(), mGameObjectIds.end(), gameObjectId);
	mGameObjectIds.erase(iter, mGameObjectIds.end());
}

int GameWorld::generateId() {
	while (mGameObjects.contains(mCurrentId)) {
		mCurrentId++;
		if (mCurrentId < 0) {
			mCurrentId = 0;
		}
	}
	return mCurrentId;
}

GameWorldData GameWorld::getData() {
	GameWorldData data;
	for (int id : mGameObjectIds) {
		data.gameObjectsData.push_back(mGameObjects[id]->getData());
	}
	return std::move(data);
}

void GameWorld::setData(const GameWorldData& data) {
	// TODO: implement setData
}
