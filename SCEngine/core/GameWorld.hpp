#ifndef _GameWorld_H_
#define _GameWorld_H_

#include <memory>

#include "box2d/box2d.h"

#include "core/GameObject.hpp"

// GameWorld stores all GameObject in the game
class GameWorld : public std::enable_shared_from_this<GameWorld> {
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
		gameObject->mGameWorld = shared_from_this();
		if (created) {
			gameObject->onCreate();
		}
	}

	float mCurrentTime = 0.0f; // time since game start
	float mDeltaTime = 0.0f;   // time spent in last frame

	b2Vec2 mGravity = b2Vec2(0.0f, -10.0f);
	b2World mPhysicsWorld = b2World(mGravity);
private:
	std::vector<std::shared_ptr<GameObject>> mGameObjects; // All GameObject in the world
	bool created = false; // Have onCreate called?
};

#endif // _GameWorld_H_
