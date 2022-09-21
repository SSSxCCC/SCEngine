#ifndef _GameObjectData_H_
#define _GameObjectData_H_

#include "data/ScriptData.h"

// Store all the data in a GameObject
struct GameObjectData {
	int id;
	std::string name;
	std::vector<ScriptData> scriptsData;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameObjectData, id, name, scriptsData)

#endif // _GameObjectData_H_
