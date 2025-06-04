#include "mapObjectsHandler.h"

void mapObjectsHandler::addGameObject( mapType type , mapObjectType objectType, std::shared_ptr<rMapObject> object ){
	std::lock_guard<std::mutex> lock( mapMutex );
	this->mapObjects[ type ][ objectType] = object;
}

void mapObjectsHandler::addTileSet( mapType type , const TileSet & tiles ){
	this->mapTiles[ type ] = tiles;
}

std::shared_ptr<rMapObject> * mapObjectsHandler::getGameObject( mapType type , mapObjectType objectType ) {
	std::lock_guard<std::mutex> lock( mapMutex );
	auto it = mapObjects.find( type );
	if(it == mapObjects.end()) {
		return nullptr; // Type not found
	}
	auto that = it->second.find( objectType );
	if(that == it->second.end()) {
		return nullptr; // Object type not found
	}

	return &that->second;
}

TileSet * mapObjectsHandler::getTileSet( mapType type )
{
	if(this->mapTiles.find( type ) != this->mapTiles.end()) {
		return &this->mapTiles[ type ];
	}

	return nullptr;
}

std::unordered_map<mapObjectType , std::shared_ptr<rMapObject>> * mapObjectsHandler::getAllMapObjects( mapType type )
{
	std::lock_guard<std::mutex> lock( mapMutex );
	auto it = mapObjects.find( type );
	if(it == mapObjects.end()) {
		return nullptr; // Type not found
	}
	return &it->second;
}
