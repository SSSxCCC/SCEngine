#ifndef _GameObject_H_
#define _GameObject_H_

#include <vector>
#include <memory>
#include "core/Script.hpp"
#include "core/Transform.hpp"

// Predefine GameWorld here to solve circular reference problem
class GameWorld;

// GameObject is a object in the game world. Attach GameObjectScript to this GameObject to implement game logic
class GameObject : public std::enable_shared_from_this<GameObject> {
public:
	void onCreate() {
		for (auto script : mScripts) {
			script->onCreate();
		}
	}

	void onUpdate() {
		for (auto script : mScripts) {
			script->onUpdate();
		}
	}

	void onDestroy() {
		for (auto script : mScripts) {
			script->onDestroy();
		}
	}

	void addScript(std::shared_ptr<Script>& script) {
		mScripts.push_back(script);
		script->mGameObject = shared_from_this();
	}

	std::shared_ptr<GameObject> clone() {
		auto gameObject = std::make_shared<GameObject>();
		for (auto script : mScripts) {
			auto newScript = script->clone();
			gameObject->addScript(newScript);
		}
		gameObject->mTransform = mTransform;
		return gameObject;
	}

	// The GameWorld this GameObject is in.
	// Note: there is circular reference between GameObject and GameWorld!
	std::shared_ptr<GameWorld> mGameWorld;

	// This GameObject's position, rotation and scale
	Transform mTransform;

private:
	// All Script attached to this GameObject
	std::vector<std::shared_ptr<Script>> mScripts;
};

#endif // _GameObject_H_
