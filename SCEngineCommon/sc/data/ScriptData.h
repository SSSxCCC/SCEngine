#ifndef _ScriptData_H_
#define _ScriptData_H_

#include <variant>
#include <memory>
#include "nlohmann/json.hpp"
#include "sc/data/Limit.h"
#include "glm/glm.hpp"

// Make glm::vec can be converted to json
namespace glm {
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(glm::vec2, x, y)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(glm::vec3, x, y, z)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(glm::vec4, x, y, z, w)
}

namespace sc {

// The type index of ScriptVar::value
// If you need add new types, you should update:
// 1. type of ScriptVar::value
// 2. to_json and from_json of ScriptVar in ScriptData.h
// 3. SceneEditor.cpp
static const int TYPE_INT = 0; // int
static const int TYPE_FLOAT = 1; // float
static const int TYPE_STRING = 2; // std::string
static const int TYPE_VEC2 = 3; // glm::vec2
static const int TYPE_VEC3 = 4; // glm::vec3
static const int TYPE_VEC4 = 5; // glm::vec4

// A single variable in a Script
struct ScriptVar {
	std::variant<int, float, std::string, glm::vec2, glm::vec3, glm::vec4> value; // the value of this variable
	std::shared_ptr<LimitBase> limit; // the limit on this variable, editor use only
	ScriptVar() { }
	template<typename T> explicit ScriptVar(const T& value, std::shared_ptr<LimitBase> limit = nullptr) : value(value), limit(limit) { }
};

// Make ScriptVar can be converted to json
inline void to_json(nlohmann::json& json, const ScriptVar& scriptVar) {
	int type = static_cast<int>(scriptVar.value.index());
	if (type == TYPE_INT) {
		json["value"] = std::get<int>(scriptVar.value);
	} else if (type == TYPE_FLOAT) {
		json["value"] = std::get<float>(scriptVar.value);
	} else if (type == TYPE_STRING) {
		json["value"] = std::get<std::string>(scriptVar.value);
	} else if (type == TYPE_VEC2) {
        json["value"] = std::get<glm::vec2>(scriptVar.value);
    } else if (type == TYPE_VEC3) {
        json["value"] = std::get<glm::vec3>(scriptVar.value);
    } else if (type == TYPE_VEC4) {
        json["value"] = std::get<glm::vec4>(scriptVar.value);
    } else throw std::runtime_error("ScriptVar::to_json unknown ScriptVar data type: " + type);
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
	} else if (type == TYPE_VEC2) {
        scriptVar.value = json["value"].get<glm::vec2>();
    } else if (type == TYPE_VEC3) {
        scriptVar.value = json["value"].get<glm::vec3>();
    } else if (type == TYPE_VEC4) {
        scriptVar.value = json["value"].get<glm::vec4>();
    } else throw std::runtime_error("ScriptVar::from_json unknown ScriptVar data type: " + type);
}

// Define how ScriptData/GameObjectData/SceneData was edited
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
        if (!dataMap.contains(name)) {
            throw std::runtime_error("ScriptData::set unknown data name: " + name);
        }
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
        return static_cast<int>(dataMap.at(name).value.index());
    }

    // get Limit, editor use only
    std::shared_ptr<LimitBase> getLimit(const std::string& name) const {
        return dataMap.at(name).limit;
    }

friend class SceneEditor;
};

} // namespace sc

#endif // _ScriptData_H_
