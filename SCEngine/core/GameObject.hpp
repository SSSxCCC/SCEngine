#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_

#include <vector>
#include <memory>
#include "core/Script.hpp"
#include "core/Transform.hpp"

// Predefine GameWorld here to solve circular reference problem
class GameWorld;

// GameObject is a object in the game world. Attach GameObjectScript to this GameObject to implement game logic
class GameObject {
public:
	GameObject(std::shared_ptr<GameWorld>& gameWorld) : mGameWorld(gameWorld) { }

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

#endif // _GAME_OBJECT_H_
