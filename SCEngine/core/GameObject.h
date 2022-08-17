#ifndef _GameObject_H_
#define _GameObject_H_

#include <vector>
#include <memory>
#include "core/Script.h"
#include "core/Transform.h"

// Predefine GameWorld here to solve circular reference problem
class GameWorld;

// GameObject is a object in the game world. Attach GameObjectScript to this GameObject to implement game logic
class GameObject : public std::enable_shared_from_this<GameObject> {
public:
	void onCreate();
	void onStart();
	void onUpdate();
	void onDestroy();

	// addScript should be called before GameWorld::addGameObject
	void addScript(const std::shared_ptr<Script>& script);

	// get first script of class S from this GameObject
	template <class S> std::shared_ptr<S> getScript() {
		for (auto script : mScripts) {
			auto s = std::dynamic_pointer_cast<S>(script);
			if (s) {
				return s;
			}
		}
		return nullptr;
	}

	// get all scripts of class S from this GameObject
	template <class S> std::vector<std::shared_ptr<S>> getScripts() {
		std::vector<std::shared_ptr<S>> vector;
		for (auto script : mScripts) {
			auto s = std::dynamic_pointer_cast<S>(script);
			if (s) {
				vector.push_back(s);
			}
		}
		return vector;
	}

	std::shared_ptr<GameObject> clone();

	// The GameWorld this GameObject is in.
	// Note: there is circular reference between GameObject and GameWorld!
	std::shared_ptr<GameWorld> mGameWorld;

	// This GameObject's position, rotation and scale
	Transform mTransform;

	bool mAdded = false;   // have been added to a GameWorld?
	bool mStarted = false; // have onStarted called?

private:
	// All Script attached to this GameObject
	std::vector<std::shared_ptr<Script>> mScripts;
};

#endif // _GameObject_H_
