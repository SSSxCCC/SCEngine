#ifndef _GameWorldData_H_
#define _GameWorldData_H_

#include "data/GameObjectData.h"

// Store all the data in a GameWorld
struct GameWorldData {
	std::vector<GameObjectData> gameObjectsData;

	// indicate how this GameWorld was edited, editor use only
	EditType editType = EditType::None;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameWorldData, gameObjectsData)

#endif // _GameWorldData_H_
