#ifndef _ScriptData_H_
#define _ScriptData_H_

#include <string>
#include <vector>
#include <variant>
#include "nlohmann/json.hpp"

// The type index of ScriptVar::value
static const int TYPE_INT = 0, TYPE_FLOAT = 1, TYPE_STRING = 2;

// A single variable in a Script
struct ScriptVar {
	std::variant<int, float, std::string> value;
	ScriptVar() { }
	template<typename T> explicit ScriptVar(const T& value) : value(value) { }
};

// Make ScriptVar can be converted to json
inline void to_json(nlohmann::json& json, const ScriptVar& scriptVar) {
	int type = scriptVar.value.index();
	if (type == TYPE_INT) {
		json["value"] = std::get<int>(scriptVar.value);
	} else if (type == TYPE_FLOAT) {
		json["value"] = std::get<float>(scriptVar.value);
	} else if (type == TYPE_STRING) {
		json["value"] = std::get<std::string>(scriptVar.value);
	} else assert(false);
	json["type"] = type;
}
inline void from_json(const nlohmann::json& json, ScriptVar& scriptVar) {
	int type = json["type"];
	if (type == TYPE_INT) {
		scriptVar.value = json["value"].get<int>();
	} else if (type == TYPE_FLOAT) {
		scriptVar.value = json["value"].get<float>();
	} else if (type == TYPE_STRING) {
		scriptVar.value = json["value"].get<std::string>();
	} else assert(false);
}

// Define how ScriptData/GameObjectData/GameWorldData was edited
enum class EditType {
	None,
	Modify,
	Move,
	Add,
	Delete,
};

// Store all the data in a Script
struct ScriptData {
public:
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(ScriptData, name, dataMap)

	// the className of Script
	std::string name;

	// indicate how this Script was edited, editor use only
	EditType editType = EditType::None;

	template<typename T> void add(const std::string& name, const T& value) { dataList.push_back(name); dataMap[name] = ScriptVar(value); }
	template<typename T> void set(const std::string& name, const T& value) { assert(dataMap.contains(name)); dataMap[name].value = value; }
	template<typename T> T get(const std::string& name) const { return std::get<T>(dataMap.at(name).value); }
	int getType(const std::string& name) const { return dataMap.at(name).value.index(); }
private:
	// key: variable name, value: variable value
	std::unordered_map<std::string, ScriptVar> dataMap;

	// store the order of variable, editor use only
	std::vector<std::string> dataList;

	friend class GameWorldEditor;
};

#endif // _ScriptData_H_
