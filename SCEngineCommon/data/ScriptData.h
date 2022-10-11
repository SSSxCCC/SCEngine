#ifndef _ScriptData_H_
#define _ScriptData_H_

#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <memory>
#include "nlohmann/json.hpp"

// The polymorphic base class of Limit
class LimitBase {
public:
	virtual ~LimitBase() { }
};

// The value limit rule for ScriptVar::value
template<typename T> class Limit : public LimitBase {
public:
	virtual T apply(const T& oldVal, const T& newVal) = 0;
};

// Limit int to some named values
class EnumLimit : public Limit<int> {
public:
	std::vector<std::string> mEnumNames;
	std::vector<int> mEnumValues;
	explicit EnumLimit(const std::vector<std::string>& enumNames) {
		std::vector<int> enumValues;
		for (int i = 0, size = enumNames.size(); i < size; i++) {
			enumValues.push_back(i);
		}
		new(this) EnumLimit(enumNames, enumValues);
	}
	EnumLimit(const std::vector<std::string>& enumNames, const std::vector<int>& enumValues) : mEnumNames(enumNames), mEnumValues(enumValues) { }
	int apply(const int& oldVal, const int& newVal) override {
		return std::find(mEnumValues.begin(), mEnumValues.end(), newVal) != mEnumValues.end() ? newVal : oldVal;
	}
};

// Limit int inside range
class IntRangeLimit : public Limit<int> {
public:
	const int mMin, mMax;
	IntRangeLimit(int min, int max) : mMin(min), mMax(max) { }
	int apply(const int& oldVal, const int& newVal) override {
		return std::min(std::max(newVal, mMin), mMax);
	}
};

// Limit float inside range
class FloatRangeLimit : public Limit<float> {
public:
	const float mMin, mMax;
	FloatRangeLimit(float min, float max) : mMin(min), mMax(max) { }
	float apply(const float& oldVal, const float& newVal) override {
		return std::min(std::max(newVal, mMin), mMax);
	}
};

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

	template<typename T> void set(const std::string& name, const T& value) {
		assert(dataMap.contains(name));
		dataMap[name].value = value;
	}

	template<typename T> T get(const std::string& name) const {
		return std::get<T>(dataMap.at(name).value);
	}

	int getType(const std::string& name) const {
		return dataMap.at(name).value.index();
	}

	std::shared_ptr<LimitBase> getLimit(const std::string& name) const {
		return dataMap.at(name).limit;
	}
private:
	// key: variable name, value: variable value
	std::unordered_map<std::string, ScriptVar> dataMap;

	// store the order of variable, editor use only
	std::vector<std::string> dataList;

	friend class GameWorldEditor;
};

#endif // _ScriptData_H_
