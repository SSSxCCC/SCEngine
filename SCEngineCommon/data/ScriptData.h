#ifndef _ScriptData_H_
#define _ScriptData_H_

#include <string>
#include <vector>
#include "nlohmann/json.hpp"

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
	void setFloat(const std::string& name, float value) { assert(floatData.contains(name)); floatData[name] = value; }
	void setInt(const std::string& name, int value) { assert(intData.contains(name)); intData[name] = value; }
	void setString(const std::string& name, const std::string& value) { assert(stringData.contains(name)); stringData[name] = value; }
private:
	// key: variable name, value: variable value
	std::unordered_map<std::string, float> floatData;
	std::unordered_map<std::string, int> intData;
	std::unordered_map<std::string, std::string> stringData;

	// store the order of variable, editor use only
	std::vector<std::string> dataList;

	friend class GameWorldEditor;
};

#endif // _ScriptData_H_
