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
public:
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScriptData, name, floatData, intData, stringData)

	// the className of Script
	std::string name;

	void addFloat(const std::string& name, float value) { dataList.push_back(name); floatData[name] = value; }
	void addInt(const std::string& name, int value) { dataList.push_back(name); intData[name] = value; }
	void addString(const std::string& name, const std::string& value) { dataList.push_back(name); stringData[name] = value; }
	float getFloat(const std::string& name) const { return floatData.at(name); }
	int getInt(const std::string& name) const { return intData.at(name); }
	std::string getString(const std::string& name) const { return stringData.at(name); }
private:
	// key: variable name, value: variable value
	std::unordered_map<std::string, float> floatData;
	std::unordered_map<std::string, int> intData;
	std::unordered_map<std::string, std::string> stringData;

	// store the order of variable, editor use only
	std::vector<std::string> dataList;

friend class GameWorldEditor;
};

/*
A script attached to a GameObject to implement game logic.

Example:
#include "core/Script.h"
class MyScript : public Script {
public:
	std::string getName() override { return "MyScript"; } // return the className
};
REGISTER_SCRIPT(MyScript)

The above example is a minimal code implementation, every non-abstract Script must:
1 inherit from Script
2 implement getName
3 call REGISTER_SCRIPT
*/
class Script : public std::enable_shared_from_this<Script> {
public:
	virtual void onCreate() { }  // called when the GameObject is created or this Script is first added to a created GameObject
	virtual void onStart() { }   // called before the first onUpdate call
	virtual void onUpdate() { }  // called every frame
	virtual void onDraw() { }    // called every frame after onUpdate
	virtual void onDestroy() { } // called when the Script is removed from GameObject or the GameObject is destroyed
	virtual std::string getName() = 0; // return the class name of this Script (we need this because C++ dosen't have reflective)
	virtual ScriptData getData() { ScriptData data; data.name = getName(); return std::move(data); } // data serialization
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

// A macro to register a Script in Script::sCreater.
// Any non-abstract Script must call this macro.
#define REGISTER_SCRIPT(ScriptClass) STATIC_BLOCK_(ScriptClass##Register) { \
	if (Script::sCreater == nullptr) { \
		Script::sCreater = new std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>; \
	} \
	(*Script::sCreater)[ScriptClass().getName()] = []() { return std::make_shared<ScriptClass>(); }; \
	return 0; \
}();

#endif // _Script_H_
