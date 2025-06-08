#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

#include "../../gameObjects/mapObject/rMapObject.h"
#include "../../../Utils/singleton.h"

#define baseTileW 16  // largura de 1 célula, em pixels
#define baseTileH 16  // altura de 1 célula, em pixels

using TileRow = std::vector<mapObjectType>;
using TileSet = std::vector<TileRow>;


// Estrutura para agrupar o TileSet e a Posição de Spawn
struct DungeonLayout {
	TileSet tiles;
	GVector2D spawnPos;
	int numRooms = 0;
};

class mapObjectsHandler : public CSingleton<mapObjectsHandler>
{
	mutable std::mutex mapMutex;
	std::unordered_map< mapType , std::unordered_map< mapObjectType , std::shared_ptr<rMapObject>>> mapObjects;
	std::unordered_map< mapType , DungeonLayout> dungeonLayout;

public:
	void addDungeonLayout( mapType type , DungeonLayout );
	DungeonLayout * getDungeonLayout( mapType type );

	void addGameObject( mapType type , mapObjectType , std::shared_ptr<rMapObject> );
	std::shared_ptr<rMapObject> * getGameObject( mapType type , mapObjectType );
	std::unordered_map< mapObjectType , std::shared_ptr<rMapObject>> * getAllMapObjects( mapType type );
};

