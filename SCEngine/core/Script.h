#ifndef _Script_H_
#define _Script_H_

#include <memory>
#include <unordered_map>
#include "nlohmann/json.hpp"
#include "utility/StaticBlock.h"

// Predefine GameObject here to solve circular reference problem
class GameObject;

// Store all the data in a Script
struct ScriptData {
	std::string name;
	std::unordered_map<std::string, std::string> data; // Key: variable name, Value: variable value
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScriptData, name, data)

/*
A script attached to a GameObject to implement game logic.

Example:
#include "core/Script.h"
class MyScript : public Script {
public:
	std::shared_ptr<Script> clone() override { return std::make_shared<MyScript>(); } // return a copy of MyScript
	std::string getName() override { return "MyScript"; } // return the className
};
REGISTER_SCRIPT(MyScript)

The above example is a minimal code implementation, every non-abstract Script must:
1 inherit from Script
2 implement clone
3 implement getName
4 call REGISTER_SCRIPT
*/
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

// Key: Script class name, Value: a funtion to create a instance of Script.
// We use this map to dynamic create instance of Script based on its name.
// (We need this because C++ dosen't have reflective)
extern std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>* gScriptCreater;

// A macro to register a Script in gScriptCreater.
// Any non-abstract Script must call this macro.
#define REGISTER_SCRIPT(ScriptClass) STATIC_BLOCK_(ScriptClass##Register) { \
	if (gScriptCreater == nullptr) { \
		gScriptCreater = new std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>; \
	} \
	(*gScriptCreater)[ScriptClass().getName()] = []() { return std::make_shared<ScriptClass>(); }; \
	return 0; \
}();

#endif // _Script_H_
