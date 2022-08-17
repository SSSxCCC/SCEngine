#include "core/GameObject.h"

void GameObject::onCreate() {
	for (auto script : mScripts) {
		script->onCreate();
	}
}

void GameObject::onStart() {
	for (auto script : mScripts) {
		script->onStart();
	}
}

void GameObject::onUpdate() {
	for (auto script : mScripts) {
		script->onUpdate();
	}
}

void GameObject::onDestroy() {
	for (auto script : mScripts) {
		script->onDestroy();
	}
}

// addScript should be called before GameWorld::addGameObject
void GameObject::addScript(const std::shared_ptr<Script>& script) {
	assert(!mAdded);
	mScripts.push_back(script);
	script->mGameObject = shared_from_this();
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
	for (auto script : mScripts) {
		auto newScript = script->clone();
		gameObject->addScript(newScript);
	}
	gameObject->mTransform = mTransform;
	return gameObject;
}
