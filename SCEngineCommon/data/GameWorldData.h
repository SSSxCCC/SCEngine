#ifndef _GameWorldData_H_
#define _GameWorldData_H_

// Store all the data in a GameWorld
struct GameWorldData {
	std::vector<GameObjectData> gameObjectsData;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameWorldData, gameObjectsData)

#endif // _GameWorldData_H_
