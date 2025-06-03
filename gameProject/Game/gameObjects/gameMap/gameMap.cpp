#include "gameMap.h"
#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"

void gameMap::init( )
{
	setMap( firstMap );
}

void gameMap::setMap( mapType type )
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	this->currentMapType = type;
	this->currentTileSet = *mapObjectsHandler::Get( ).getTileSet( type );
	this->currentMapObjects = mapObjectsHandler::Get( ).getAllMapObjects( type );
}

mapType gameMap::getCurrentMapType( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->currentMapType;
}

mapTileSet gameMap::getCurrentTileSet( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->currentTileSet;
}

std::unordered_map<mapObjectType , std::shared_ptr<rMapObject>> * gameMap::getCurrentMapObjects( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->currentMapObjects;
}

GVector2D gameMap::getMapPosition( ) const
{
	std::lock_guard<std::mutex> lock( this->mapMutex );
	return this->mapPosition;
}
