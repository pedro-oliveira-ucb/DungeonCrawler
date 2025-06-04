#pragma once
#include <unordered_map>
#include <mutex>
#include "../../../Utils/singleton.h"

#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"
#include "../../SDK/math/Vector2D/GVector2D.h"


class gameMap : public CSingleton<gameMap>
{
private:
	mutable std::mutex mapMutex;
	mapType currentMapType = firstMap;
	TileSet  currentTileSet;
	std::unordered_map < mapObjectType , std::shared_ptr<rMapObject>> * currentMapObjects;
	GVector2D mapPosition = GVector2D( 0 , 0 );
public:
	void init();
	// Load the map from a file
	void setMap( mapType );
	mapType getCurrentMapType( ) const;
	TileSet getCurrentTileSet( ) const;
	// Get the current map objects
	std::unordered_map < mapObjectType , std::shared_ptr<rMapObject>> * getCurrentMapObjects( ) const;
	GVector2D getMapPosition( ) const;

	bool inInMap( const GVector2D & position ) const;
};

