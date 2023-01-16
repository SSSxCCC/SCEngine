#include "sc/core/Scene.h"
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace sc {

void Scene::create() {
    for (const auto& [_, gameObject] : mGameObjects) {
        gameObject->create();
    }
    mCreated = true;
}

void Scene::update() {
    for (const auto& [_, gameObject] : mGameObjects) {
        gameObject->update();
    }
}

void Scene::draw(const DrawData& drawData) {
    for (const auto& [_, gameObject] : mGameObjects) {
        gameObject->draw(drawData);
    }
}

void Scene::destroy() {
    while (mGameObjectIds.size() > 0) {
        removeGameObject(mGameObjectIds[mGameObjectIds.size() - 1]);
    }
    if (mGameObjectIds.size() != 0) throw std::runtime_error("Scene::destroy mGameObjectIds.size() is not zero, it is " + mGameObjectIds.size());
    if (mGameObjects.size() != 0) throw std::runtime_error("Scene::destroy mGameObjects.size() is not zero, it is " + mGameObjects.size());
}

void Scene::addGameObject(const std::shared_ptr<GameObject>& gameObject) {
    if (gameObject->mId < 0) {
        gameObject->mId = generateId();
    } else if (mGameObjects.contains(gameObject->mId)) {
        throw std::runtime_error("Scene::addGameObject GameObject id already exists!");
    }
    mGameObjectIds.push_back(gameObject->mId);
    mGameObjects[gameObject->mId] = gameObject;
    gameObject->mScene = shared_from_this();
    if (mCreated) {
        gameObject->create();
    }
}

void Scene::removeGameObject(int gameObjectId) {
    if (!mGameObjects.contains(gameObjectId)) {
        std::cout << "Scene::removeGameObject GameObjectId is not exist, GameObjectId=" << gameObjectId << std::endl;
        return;
    }
    mGameObjects[gameObjectId]->destroy();
    mGameObjects[gameObjectId]->mScene = nullptr;
    mGameObjects.erase(gameObjectId);
    auto iter = std::remove(mGameObjectIds.begin(), mGameObjectIds.end(), gameObjectId);
    mGameObjectIds.erase(iter, mGameObjectIds.end());
}

int Scene::generateId() {
    while (mGameObjects.contains(mCurrentId)) {
        mCurrentId++;
        if (mCurrentId < 0) {
            mCurrentId = 0;
        }
    }
    return mCurrentId;
}

SceneData Scene::getData() {
    SceneData data;
    for (int id : mGameObjectIds) {
        data.gameObjectsData.push_back(mGameObjects[id]->getData());
    }
    return std::move(data);
}

void Scene::setData(const SceneData& sceneData) {
	for (const auto& gameObjectData : sceneData.gameObjectsData) {
		if (gameObjectData.editType == EditType::Modify) {
			mGameObjects[gameObjectData.id]->setData(gameObjectData);
		} // TODO: hanle other edit type
	}
}

std::shared_ptr<Scene> Scene::create(const SceneData& data, Engine* engine) {
	auto scene = std::make_shared<Scene>(engine);
	for (const auto& gameObjectData : data.gameObjectsData) {
		scene->addGameObject(GameObject::create(gameObjectData));
	}
	return scene;
}

} // namespace sc
