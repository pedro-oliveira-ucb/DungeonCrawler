#pragma once
#include <mutex>
#include <unordered_map>
#include "../../gameObjects/mapObject/rMapObject.h"
#include "../../gameObjects/gameRoomLevel/gameRoomLevel.h"

#include "../../../Utils/singleton.h"

class LevelHandler : public CSingleton<LevelHandler>
{
	mutable std::mutex m_mutex;
	std::unordered_map<mapType , std::vector<gameRoomLevel>> m_levels;
	

public:
	void addLevel( mapType type , gameRoomLevel level );
	void clearLevels( );
	std::vector<gameRoomLevel> * getMapRoomLevels( mapType type );
};

