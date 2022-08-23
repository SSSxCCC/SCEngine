#ifndef _Script_H_
#define _Script_H_

#include <memory>
#include <unordered_map>
#include "nlohmann/json.hpp"

// Predefine GameObject here to solve circular reference problem
class GameObject;

struct ScriptData {
	std::string name;
	std::unordered_map<std::string, std::string> data; // Key: variable name, Value: variable value
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScriptData, name, data)

// Script is a script attached to a GameObject to implement game logic
class Script : public std::enable_shared_from_this<Script> {
public:
	virtual void onCreate() { }  // called when the GameObject is created or this Script is first added to a created GameObject
	virtual void onStart() { }   // called before the first onUpdate call
	virtual void onUpdate() { }  // called every frame
	virtual void onDraw() { }    // called every frame after onUpdate
	virtual void onDestroy() { } // called when the Script is removed from GameObject or the GameObject is destroyed

	virtual std::shared_ptr<Script> clone() = 0; // create a copy of this Script

	virtual std::string getName() = 0; // return the class name of this Script (we need this because C++ dosen't have reflective)

	virtual ScriptData getData() { ScriptData data; data.name = getName(); return std::move(data); }
	virtual void setData(const ScriptData& data) { }

	// the GameObject this Script attached to.
	// Note: there is circular reference between Script and GameObject!
	std::shared_ptr<GameObject> mGameObject;

	bool mStarted = false; // have onStart called?
};

#endif // _Script_H_
