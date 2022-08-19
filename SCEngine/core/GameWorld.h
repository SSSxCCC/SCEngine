#ifndef _GameWorld_H_
#define _GameWorld_H_

#include <memory>

#include "box2d/box2d.h"

#include "core/GameObject.h"
#include "core/Camera.h"
#include "editor/DebugDraw.h"

using GameWorldData = std::unordered_map<int, GameObjectData>;

// GameWorld stores all GameObject in the game
class GameWorld : public std::enable_shared_from_this<GameWorld> {
public:
	void create();
	void update();
	void destroy();
	void addGameObject(const std::shared_ptr<GameObject>& gameObject);
	GameWorldData getData();
	void setData(const GameWorldData& data);

	float mCurrentTime = 0.0f; // time since game start
	float mDeltaTime = 0.0f;   // time spent in last frame

	int velocityIterations = 8;
	int positionIterations = 3;

	b2Vec2 mGravity = b2Vec2(0.0f, -10.0f);
	b2World mPhysicsWorld = b2World(mGravity); // the box2d physics world
	DebugDraw mDebugDraw;

	std::shared_ptr<Camera> mMainCamera;
private:
	// generate a unique id for new added GameObject
	int generateId();
	int mCurrentId = 0;

	std::unordered_map<int, std::shared_ptr<GameObject>> mGameObjects; // All GameObject in the world, key is the id of GameObject
	bool mCreated = false; // have create called?
};

#endif // _GameWorld_H_
