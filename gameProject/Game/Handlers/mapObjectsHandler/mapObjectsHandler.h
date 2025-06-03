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

#define mapTileSet std::vector<std::vector<mapObjectType>>

class mapObjectsHandler : public CSingleton<mapObjectsHandler>
{
	std::unordered_map< mapType , std::unordered_map< mapObjectType , std::shared_ptr<rMapObject>>> mapObjects;
	std::unordered_map< mapType , mapTileSet> mapTiles;
	mutable std::mutex mapMutex;
public:
	void addGameObject( mapType type , mapObjectType , std::shared_ptr<rMapObject> );
	void addTileSet( mapType type , const mapTileSet & tiles );

	std::shared_ptr<rMapObject> * getGameObject( mapType type , mapObjectType );
	mapTileSet * getTileSet( mapType type );
	std::unordered_map< mapObjectType , std::shared_ptr<rMapObject>> * getAllMapObjects( mapType type );

};

