#ifndef _GameObject_H_
#define _GameObject_H_

#include <vector>
#include <memory>
#include "core/Script.h"
#include "data/GameObjectData.h"

// Predefine GameWorld here to solve circular reference problem
class GameWorld;

// GameObject is a object in the game world. Attach GameObjectScript to this GameObject to implement game logic
class GameObject : public std::enable_shared_from_this<GameObject> {
public:
	GameObject(const std::string& name) : mName(name) { }
	void create();
	void update();
	void draw(const DrawData& drawData);
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

	// get first script of name from this GameObject
	std::shared_ptr<Script> getScript(const std::string& name);

	// create a copy of this GameObject, its mId is -1
	std::shared_ptr<GameObject> clone();

	// serialize data
	GameObjectData getData();
	void setData(const GameObjectData& gameObjectData);

	// create a instance of GameObject based on GameObjectData
	static std::shared_ptr<GameObject> create(const GameObjectData& data);

	// The GameWorld this GameObject is in.
	// Note: there is circular reference between GameObject and GameWorld!
	std::shared_ptr<GameWorld> mGameWorld;

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
