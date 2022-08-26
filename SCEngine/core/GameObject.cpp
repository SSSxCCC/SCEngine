#include "core/GameObject.h"

void GameObject::onCreate() {
	for (const auto& script : mScripts) {
		script->onCreate();
	}
	mCreated = true;
}

void GameObject::onUpdate() {
	for (const auto& script : mScripts) {
		if (!script->mStarted) {
			// call Script::onStart once before Script::onUpdate get first called
			script->onStart();
			script->mStarted = true;
		}
		script->onUpdate();
	}
	// call Script::onDraw every frame after Script::onUpdate get called
	for (const auto& script : mScripts) {
		script->onDraw();
	}
}

void GameObject::onDestroy() {
	for (const auto& script : mScripts) {
		script->onDestroy();
		script->mGameObject = nullptr;
	}
}

// addScript should be called before GameWorld::addGameObject
void GameObject::addScript(const std::shared_ptr<Script>& script) {
	mScripts.push_back(script);
	script->mGameObject = shared_from_this();
	if (mCreated) {
		script->onCreate();
	}
}

std::shared_ptr<GameObject> GameObject::clone() {
	auto data = getData();
	data.id = -1;
	return create(data);
}

GameObjectData GameObject::getData() {
	GameObjectData data;
	data.id = mId;
	data.name = mName;
	data.transform = mTransform;
	for (const auto& script : mScripts) {
		data.scriptsData.push_back(script->getData());
	}
	return std::move(data);
}

std::shared_ptr<GameObject> GameObject::create(const GameObjectData& data) {
	auto gameObject = std::make_shared<GameObject>();
	gameObject->mId = data.id;
	gameObject->mName = data.name;
	gameObject->mTransform = data.transform;
	for (const auto& scriptData : data.scriptsData) {
		auto script = Script::create(scriptData);
		gameObject->addScript(script);
	}
	return gameObject;
}
