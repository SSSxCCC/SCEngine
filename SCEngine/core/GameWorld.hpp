#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include "core/GameObject.hpp"

// GameWorld stores all GameObject in the game
class GameWorld {
public:
	void create() {
		created = true;
		for (auto gameObject : mGameObjects) {
			gameObject->onCreate();
		}
	}

	void update() {
		for (auto gameObject : mGameObjects) {
			gameObject->onUpdate();
		}
	}

	void addGameObject(std::shared_ptr<GameObject>& gameObject) {
		mGameObjects.push_back(gameObject);
		if (created) {
			gameObject->onCreate();
		}
	}

	float mCurrentTime = 0.0f; // time since game start
	float mDeltaTime = 0.0f;   // time spent in last frame
private:
	std::vector<std::shared_ptr<GameObject>> mGameObjects; // All GameObject in the world
	bool created = false; // Have onCreate called?
};

#endif // _GAME_WORLD_H_
