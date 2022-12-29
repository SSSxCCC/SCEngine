#ifndef _GameWorld_H_
#define _GameWorld_H_

#include <memory>

#include "physics2d/PhysicsWorld2D.h"
#include "core/GameObject.h"
#include "core/Camera.h"
#include "data/GameWorldData.h"

// Predefine SCEngine here to solve circular reference problem
class SCEngine;

// GameWorld stores all GameObject in the game
class GameWorld : public std::enable_shared_from_this<GameWorld> {
public:
	GameWorld(SCEngine* pSCEngine) : mSCEngine(pSCEngine) { };
	void create();
	void update();
	void draw(const DrawData& drawData);
	void destroy();
	void addGameObject(const std::shared_ptr<GameObject>& gameObject);
	void removeGameObject(int GameObjectId);
	GameWorldData getData();
	void setData(const GameWorldData& gameWorldData);

	// create a instance of GameWorld based on GameWorldData
	static std::shared_ptr<GameWorld> create(const GameWorldData& data, SCEngine* pSCEngine);

	const SCEngine* mSCEngine; // global variables and functions

	float mCurrentTime = 0.0f; // time since game start
	float mDeltaTime = 0.0f;   // time spent in last frame

	std::shared_ptr<PhysicsWorld2D> mPhysicsWorld2D; // the box2d physics world wrapper
	std::shared_ptr<Camera> mMainCamera; // the camera to display the game
	std::shared_ptr<Camera> mEditorCamera; // the camera to display the editor
private:
	// generate a unique id for new added GameObject
	int generateId();
	int mCurrentId = 0;

	std::vector<int> mGameObjectIds; // all GameObject ids in a order
	std::unordered_map<int, std::shared_ptr<GameObject>> mGameObjects; // all GameObject in the world, key is the id of GameObject
	bool mCreated = false; // have create called?

friend class GameWorldEditor;
};

#endif // _GameWorld_H_
