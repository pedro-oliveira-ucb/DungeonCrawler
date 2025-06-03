#include "mapObjectsInitializer.h"

#include  "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"

#include "../../../Utils/Log/Log.h"

#include "../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"

bool  initializeMapObjects( mapType map , std::string mapName, mapTileSet set )
{
	for ( int i = 1; i < ( int ) mapObjectType::mapObjectCount; i++ ) {
		mapObjectType objectType = ( mapObjectType ) i;
		std::string objectTypeName = rMapObject::geMapObjectTypeName( objectType );
		Log::Print( "Initializing map objects of type: %s for map: %s" , objectTypeName.c_str( ) , mapName.c_str( ) );

		std::shared_ptr<rSprite> mapAsset =
			_gameResourceManager.getMapManager( )->getMapAsset( mapName + "_" + objectTypeName );

		if ( mapAsset == nullptr || mapAsset.get( ) == nullptr ) {
			Log::Print( "Failed to load map asset for type: %s in map: %s" , objectTypeName.c_str( ) , mapName.c_str( ) );
			return false;
		}
		std::shared_ptr<rMapObject> mapObject = std::make_shared<rMapObject>( objectType , mapAsset );
		mapObjectsHandler::Get( ).addGameObject( map , objectType , mapObject );
	}
	mapObjectsHandler::Get( ).addTileSet( map , set );

	return true;
}

bool mapObjectsInitializer::initialize( ) {
	// Load the map objects from the specified map file

	mapObjectType d = mapObjectType::door;
	mapObjectType t = mapObjectType::tallWall;
	mapObjectType s = mapObjectType::shortWall;
	mapObjectType lt = mapObjectType::lateralTallWall;
	mapObjectType ls = mapObjectType::lateralShortWall;
	mapObjectType g = mapObjectType::ground;
	mapObjectType n = mapObjectType::mapObjectNone;

	mapTileSet tile =
	{	
		{lt,t,t,t,d,t,t,t,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,t,t,t,t,t,t,t,lt}
	};

	std::vector<std::pair<mapType , std::string>> maps = {
		{ mapType::firstMap, "firstMap" },
		{ mapType::secondMap, "secondMap" },
		{ mapType::thirdMap, "thirdMap" },
		{ mapType::bossMap, "bossMap" },
	};

	for ( auto & map : maps ) {
		if ( !initializeMapObjects( map.first , map.second, tile ) ) {
			Log::Print( "Failed to initialize map objects for %s" , map.second.c_str( ) );
			return false;
		}
	}

	return true; // All map objects initialized successfully
}