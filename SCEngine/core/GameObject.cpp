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

// get first script of class S from this GameObject
//template <class S> std::shared_ptr<S> GameObject::getScript() {
//	for (auto script : mScripts) {
//		auto s = std::dynamic_pointer_cast<S>(script);
//		if (s) {
//			return s;
//		}
//	}
//	return nullptr;
//}

// get all scripts of class S from this GameObject
//template <class S> std::vector<std::shared_ptr<S>> GameObject::getScripts() {
//	std::vector<std::shared_ptr<S>> vector;
//	for (auto script : mScripts) {
//		auto s = std::dynamic_pointer_cast<S>(script);
//		if (s) {
//			vector.push_back(s);
//		}
//	}
//	return vector;
//}

std::shared_ptr<GameObject> GameObject::clone() {
	auto gameObject = std::make_shared<GameObject>();
	for (const auto& script : mScripts) {
		auto newScript = script->clone();
		gameObject->addScript(newScript);
	}
	gameObject->mTransform = mTransform;
	return gameObject;
}

GameObjectData GameObject::getData() {
	GameObjectData data;
	for (const auto& script : mScripts) {
		data[script->getName()] = script->getData();
	}
	return std::move(data);
}

void GameObject::setData(const GameObjectData& data) {
	for (const auto& script : mScripts) {
		script->setData(data.at(script->getName()));
	}
}
