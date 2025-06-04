#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>

#include "../../gameObjects/mapObject/rMapObject.h"
#include "../../../Utils/singleton.h"

enum mapType {
	firstMap ,
	secondMap ,
	thirdMap ,
	bossMap
};

using TileRow = std::vector<mapObjectType>;
using TileSet = std::vector<TileRow>;

#define baseTileW 16  // largura de 1 célula, em pixels
#define baseTileH 16  // altura de 1 célula, em pixels

class mapObjectsHandler : public CSingleton<mapObjectsHandler>
{
	std::unordered_map< mapType , std::unordered_map< mapObjectType , std::shared_ptr<rMapObject>>> mapObjects;
	std::unordered_map< mapType , TileSet> mapTiles;
	mutable std::mutex mapMutex;
public:
	void addGameObject( mapType type , mapObjectType , std::shared_ptr<rMapObject> );
	void addTileSet( mapType type , const TileSet & tiles );

	std::shared_ptr<rMapObject> * getGameObject( mapType type , mapObjectType );
	TileSet * getTileSet( mapType type );
	std::unordered_map< mapObjectType , std::shared_ptr<rMapObject>> * getAllMapObjects( mapType type );

};

