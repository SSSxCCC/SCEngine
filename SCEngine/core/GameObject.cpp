#include "core/GameObject.h"

void GameObject::create() {
	for (const auto& script : mScripts) {
		script->onCreate();
	}
	mCreated = true;
}

void GameObject::update() {
	for (const auto& script : mScripts) {
		if (!script->mStarted) {
			// call Script::onStart once before Script::onUpdate get first called
			script->onStart();
			script->mStarted = true;
		}
		script->onUpdate();
	}
}

void GameObject::draw(float* projectionMatrix, bool forEditor) {
	for (const auto& script : mScripts) {
		script->onDraw(projectionMatrix, forEditor);
	}
}

void GameObject::destroy() {
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
	for (const auto& script : mScripts) {
		data.scriptsData.push_back(script->getData());
	}
	return data;
}

std::shared_ptr<GameObject> GameObject::create(const GameObjectData& data) {
	auto gameObject = std::make_shared<GameObject>(data.name);
	gameObject->mId = data.id;
	for (const auto& scriptData : data.scriptsData) {
		auto script = Script::create(scriptData);
		gameObject->addScript(script);
	}
	return gameObject;
}
