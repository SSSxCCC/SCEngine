#ifndef _ScriptData_H_
#define _ScriptData_H_

#include <variant>
#include <memory>
#include "nlohmann/json.hpp"
#include "data/Limit.h"

// The type index of ScriptVar::value
static const int TYPE_INT = 0, TYPE_FLOAT = 1, TYPE_STRING = 2;

// A single variable in a Script
struct ScriptVar {
	std::variant<int, float, std::string> value; // the value of this variable
	std::shared_ptr<LimitBase> limit; // the limit on this variable, editor use only
	ScriptVar() { }
	template<typename T> explicit ScriptVar(const T& value, std::shared_ptr<LimitBase> limit = nullptr) : value(value), limit(limit) { }
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

	template<typename T> void add(const std::string& name, const T& value, std::shared_ptr<LimitBase> limit = nullptr) {
		dataList.push_back(name);
		dataMap[name] = ScriptVar(value, limit);
	}

	template<typename T> T get(const std::string& name) const {
		return std::get<T>(dataMap.at(name).value);
	}
private:
	// key: variable name, value: variable value
	std::unordered_map<std::string, ScriptVar> dataMap;

	// store the order of variable, editor use only
	std::vector<std::string> dataList;

    // set data by editor, the data is not necessarily updated because Limit, return true if the data has been updated
    template<typename T> bool set(const std::string& name, const T& value) {
        assert(dataMap.contains(name));
        T oldValue = std::get<T>(dataMap[name].value);
        T newValue = value;
        auto baseLimit = getLimit(name);
        if (auto limit = std::dynamic_pointer_cast<Limit<T>>(baseLimit)) {
            newValue = limit->apply(oldValue, newValue);
        }
        if (oldValue != newValue) {
            dataMap[name].value = value;
            return true;
        }
        return false;
    }

    // get data type, editor use only
    int getType(const std::string& name) const {
        return dataMap.at(name).value.index();
    }

    // get Limit, editor use only
    std::shared_ptr<LimitBase> getLimit(const std::string& name) const {
        return dataMap.at(name).limit;
    }

	friend class GameWorldEditor;
};

#endif // _ScriptData_H_
