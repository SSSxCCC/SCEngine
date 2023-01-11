#ifndef _Scene_H_
#define _Scene_H_

#include <memory>

#include "sc/physics2d/PhysicsWorld2D.h"
#include "sc/core/GameObject.h"
#include "sc/core/Camera.h"
#include "sc/data/SceneData.h"

namespace sc {

// Predefine SCEngine here to solve circular reference problem
class SCEngine;

// Scene includes many GameObjects
class Scene : public std::enable_shared_from_this<Scene> {
public:
	Scene(SCEngine* pSCEngine) : mSCEngine(pSCEngine) { };
	void create();
	void update();
	void draw(const DrawData& drawData);
	void destroy();
	void addGameObject(const std::shared_ptr<GameObject>& gameObject);
	void removeGameObject(int GameObjectId);
	SceneData getData();
	void setData(const SceneData& sceneData);

	// create a instance of Scene based on SceneData
	static std::shared_ptr<Scene> create(const SceneData& data, SCEngine* pSCEngine);

	const SCEngine* mSCEngine; // global variables and functions

	float mCurrentTime = 0.0f; // time since scene start
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

friend class SceneEditor;
};

} // namespace sc

#endif // _Scene_H_
