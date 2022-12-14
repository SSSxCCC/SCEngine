#include "core/GameWorld.h"
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

void GameWorld::create() {
	for (const auto& [_, gameObject] : mGameObjects) {
		gameObject->create();
	}
	mCreated = true;
}

void GameWorld::update() {
	for (const auto& [_, gameObject] : mGameObjects) {
		gameObject->update();
	}
}

void GameWorld::draw(const DrawData& drawData) {
	for (const auto& [_, gameObject] : mGameObjects) {
		gameObject->draw(drawData);
	}
}

void GameWorld::destroy() {
	while (mGameObjectIds.size() > 0) {
		removeGameObject(mGameObjectIds[mGameObjectIds.size() - 1]);
	}
	assert(mGameObjectIds.size() == 0);
	assert(mGameObjects.size() == 0);
}

void GameWorld::addGameObject(const std::shared_ptr<GameObject>& gameObject) {
	if (gameObject->mId < 0) {
		gameObject->mId = generateId();
	} else {
		assert(!mGameObjects.contains(gameObject->mId));
	}
	mGameObjectIds.push_back(gameObject->mId);
	mGameObjects[gameObject->mId] = gameObject;
	gameObject->mGameWorld = shared_from_this();
	if (mCreated) {
		gameObject->create();
	}
}

void GameWorld::removeGameObject(int gameObjectId) {
	if (!mGameObjects.contains(gameObjectId)) {
		std::cout << "GameWorld::removeGameObject GameObjectId is not exist, GameObjectId=" << gameObjectId << std::endl;
		return;
	}
	mGameObjects[gameObjectId]->destroy();
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

void GameWorld::setData(const GameWorldData& gameWorldData) {
	for (const auto& gameObjectData : gameWorldData.gameObjectsData) {
		if (gameObjectData.editType == EditType::Modify) {
			mGameObjects[gameObjectData.id]->setData(gameObjectData);
		} // TODO: hanle other edit type
	}
}

std::shared_ptr<GameWorld> GameWorld::create(const GameWorldData& data, SCEngine* pSCEngine) {
	auto gameWorld = std::make_shared<GameWorld>(pSCEngine);
	for (const auto& gameObjectData : data.gameObjectsData) {
		gameWorld->addGameObject(GameObject::create(gameObjectData));
	}
	return gameWorld;
}
