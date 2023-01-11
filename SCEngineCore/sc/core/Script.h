#ifndef _Script_H_
#define _Script_H_

#include <memory>
#include <unordered_map>
#include "sc/utility/StaticBlock.h"
#include "sc/data/ScriptData.h"
#include "sc/graphics/DrawData.h"

namespace sc {

// Predefine GameObject here to solve circular reference problem
class GameObject;

/*
A script attached to a GameObject to implement game logic.

Example:
#include "core/Script.h"
class MyScript : public Script {
public:
    SCRIPT_BODY(MyScript)
};
REGISTER_SCRIPT(MyScript)

The above example is a minimal code implementation, every non-abstract Script must:
1 inherit from Script
2 implement getName
3 call REGISTER_SCRIPT
*/
class Script : public std::enable_shared_from_this<Script> {
public:
	virtual void onCreate() { } // called when the GameObject is created or this Script is first added to a created GameObject
	virtual void onStart() { } // called before the first onUpdate call
	virtual void onUpdate() { } // called every frame
	virtual void onDraw(const DrawData& drawData) { } // called every frame after onUpdate
	virtual void onDestroy() { } // called when the Script is removed from GameObject or the GameObject is destroyed
	virtual std::string getName() = 0; // return the class name of this Script (we need this because C++ dosen't have reflective)
	virtual ScriptData getData() { ScriptData data; data.name = getName(); return data; } // data serialization
	virtual void setData(const ScriptData& data) { } // data deserialization

	// create a copy of this Script
	std::shared_ptr<Script> clone() { return create(getData()); }

	// the GameObject this Script attached to.
	// Note: there is circular reference between Script and GameObject!
	std::shared_ptr<GameObject> mGameObject;

	// have onStart called?
	bool mStarted = false;

	// Key: Script class name, Value: a funtion to create a instance of Script.
	// We use this map to dynamic create instance of Script based on its name.
	// (We need this because C++ dosen't have reflective)
	static std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>* sCreater;

	// create a instance of Script based on ScriptData
	static std::shared_ptr<Script> create(const ScriptData& data);
};

// A macro to define ScriptClass::getName method and ScriptClass::name static method.
// Any non-abstract Script must call this macro in the public area of the class.
#define SCRIPT_BODY(ScriptClass) \
    std::string getName() override { return #ScriptClass; } \
    static std::string name() { return #ScriptClass; }

// A macro to register a Script in Script::sCreater.
// Any non-abstract Script must call this macro after the class definition.
#define REGISTER_SCRIPT(ScriptClass) STATIC_BLOCK_(ScriptClass##Register) { \
	if (Script::sCreater == nullptr) { \
		Script::sCreater = new std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>; \
	} \
	(*Script::sCreater)[ScriptClass::name()] = []() { return std::make_shared<ScriptClass>(); }; \
	return 0; \
}();

} // namespace sc

#endif // _Script_H_
