#ifndef _GameWorld_H_
#define _GameWorld_H_

#include <memory>

#include "box2d/box2d.h"

#include "DebugDraw.hpp"
#include "core/GameObject.hpp"
#include "core/Camera.hpp"

// GameWorld stores all GameObject in the game
class GameWorld : public std::enable_shared_from_this<GameWorld> {
public:
	void create() {
		created = true;
		for (auto gameObject : mGameObjects) {
			gameObject->onCreate();
		}
		mDebugDraw.Create();
		mPhysicsWorld.SetDebugDraw(&mDebugDraw);
	}

	void update() {
		// First update physics world
		mPhysicsWorld.Step(mDeltaTime, velocityIterations, positionIterations);

		// Then update game objects
		for (auto gameObject : mGameObjects) {
			if (!gameObject->mStarted) {
				gameObject->onStart();
				gameObject->mStarted = true;
			}
			gameObject->onUpdate();
		}
		gCamera.onUpdate();

		// Last debug draw the physics objects
		mDebugDraw.SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit | b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit);
		mPhysicsWorld.DebugDraw();
		mDebugDraw.Flush();
	}

	void destroy() {
		mDebugDraw.Destroy();
		// TODO: destroy all gameObjects
	}

	void addGameObject(const std::shared_ptr<GameObject>& gameObject) {
		mGameObjects.push_back(gameObject);
		gameObject->mGameWorld = shared_from_this();
		gameObject->mAdded = true;
		if (created) {
			gameObject->onCreate();
		}
	}

	float mCurrentTime = 0.0f; // time since game start
	float mDeltaTime = 0.0f;   // time spent in last frame

	int velocityIterations = 8;
	int positionIterations = 3;

	b2Vec2 mGravity = b2Vec2(0.0f, -10.0f);
	b2World mPhysicsWorld = b2World(mGravity); // the box2d physics world
	DebugDraw mDebugDraw;
private:
	std::vector<std::shared_ptr<GameObject>> mGameObjects; // All GameObject in the world
	bool created = false; // Have onCreate called?
};

#endif // _GameWorld_H_
