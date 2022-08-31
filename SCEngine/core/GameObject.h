#ifndef _GameObject_H_
#define _GameObject_H_

#include <vector>
#include <memory>
#include "core/Script.h"
#include "core/Transform.h"

// Predefine GameWorld here to solve circular reference problem
class GameWorld;

// Store all the data in a GameObject
struct GameObjectData {
	int id;
	std::string name;
	Transform transform;
	std::vector<ScriptData> scriptsData;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameObjectData, id, name, transform, scriptsData)

// GameObject is a object in the game world. Attach GameObjectScript to this GameObject to implement game logic
class GameObject : public std::enable_shared_from_this<GameObject> {
public:
	void create();
	void update();
	void draw(float* projectionMatrix, bool forEditor = false);
	void destroy();

	// add a Script to this GameObject
	// warning: addScript should not add two scripts of same type
	void addScript(const std::shared_ptr<Script>& script);

	// get first script of class S from this GameObject
	template <class S> std::shared_ptr<S> getScript() {
		for (const auto& script : mScripts) {
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
		for (const auto& script : mScripts) {
			auto s = std::dynamic_pointer_cast<S>(script);
			if (s) {
				vector.push_back(s);
			}
		}
		return vector;
	}

	// create a copy of this GameObject, its mId is -1
	std::shared_ptr<GameObject> clone();

	// serialize data
	GameObjectData getData();

	// create a instance of GameObject based on GameObjectData
	static std::shared_ptr<GameObject> create(const GameObjectData& data);

	// The GameWorld this GameObject is in.
	// Note: there is circular reference between GameObject and GameWorld!
	std::shared_ptr<GameWorld> mGameWorld;

	Transform mTransform; // this GameObject's position, rotation and scale
	std::string mName;    // the name of this GameObject
	int mId = -1;         // the unique id of this GameObject, -1 means there is no id yet

private:
	// All Script attached to this GameObject
	std::vector<std::shared_ptr<Script>> mScripts;

	// have onCreate called?
	bool mCreated = false;

friend class GameWorldEditor;
};

#endif // _GameObject_H_
