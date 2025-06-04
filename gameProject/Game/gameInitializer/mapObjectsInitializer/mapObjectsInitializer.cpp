#include "mapObjectsInitializer.h"

#include  "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"

#include "../../../Utils/Log/Log.h"

#include "../../Managers/gameResourceManager/gameResourceManager.h"
#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"

using namespace std;

bool  initializeMapObjects( mapType map , std::string mapName )
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


	return true;
}

void addMapTileset( mapType map , TileSet set )
{
	mapObjectsHandler::Get( ).addTileSet( map , set );
}




// Cada sprite de mapObjectType ocupa N células VERTICAIS (= altura total / 16)
// (por exemplo, ground = 16px → 1 célula; tallWall = 64px → 4 células)
static unordered_map<mapObjectType , int> spriteHeightInCells = {
	{ mapObjectType::mapObjectNone           , 0 },  // nunca vamos renderizar/contar, mas definimos 0
	{ mapObjectType::tallWall                , 4 },  // 64px / 16px = 4 células
	{ mapObjectType::rightlateralTallWall    , 4 },  // 64px / 16px = 4 células
	{ mapObjectType::leftlateralTallWall     , 4 },  // 64px / 16px = 4 células
	{ mapObjectType::ground                  , 1 },  // 16px / 16px = 1 célula
	{ mapObjectType::door                    , 4 }   // 64px / 16px = 4 células
};

// Cada sprite de mapObjectType ocupa N células HORIZONTAIS (= largura total / 16)
// (por exemplo, ground = 16px → 1 célula; tallWall = 64px → 4 células)
static unordered_map<mapObjectType , int> spriteWidthInCells = {
	{ mapObjectType::mapObjectNone           , 0 },
	{ mapObjectType::tallWall                , 4 },  // 64px / 16px = 4 células
	{ mapObjectType::rightlateralTallWall    , 4 },  // 64px / 16px = 4 células
	{ mapObjectType::leftlateralTallWall     , 4 },  // 64px / 16px = 4 células
	{ mapObjectType::ground                  , 1 },  // 16px / 16px = 1 célula
	{ mapObjectType::door                    , 4 }   // 64px / 16px = 4 células
};

TileSet corrigirTilesetPorCelulas(
	const TileSet & original ,
	unordered_map<mapObjectType , shared_ptr<rMapObject>> * tileObjects
) {

	//Descobre quantas colunas (em termos de entradas originais) existem
	size_t maxCols = 0;
	for ( auto const & row : original ) {
		if ( row.size( ) > maxCols )
			maxCols = row.size( );
	}

	// Normaliza cada linha para ter exatamente maxCols entradas,
	// preenchendo com mapObjectNone se faltar.
	TileSet normalized = original;
	for ( auto & row : normalized ) {
		if ( row.size( ) < maxCols ) {
			row.resize( maxCols , mapObjectType::mapObjectNone );
		}
	}

	return normalized;
}


bool mapObjectsInitializer::initialize( ) {
	// Load the map objects from the specified map file


	std::vector<std::pair<mapType , std::string>> maps = {
		{ mapType::firstMap, "firstMap" },
		{ mapType::secondMap, "secondMap" },
		{ mapType::thirdMap, "thirdMap" },
		{ mapType::bossMap, "bossMap" },
	};

	for ( auto & map : maps ) {
		if ( !initializeMapObjects( map.first , map.second ) ) {
			Log::Print( "Failed to initialize map objects for %s" , map.second.c_str( ) );
			return false;
		}


		mapObjectType d = mapObjectType::door;
		mapObjectType t = mapObjectType::tallWall;
		mapObjectType g = mapObjectType::ground;
		mapObjectType n = mapObjectType::mapObjectNone;
		mapObjectType r = mapObjectType::rightlateralTallWall;
		mapObjectType l = mapObjectType::leftlateralTallWall;

		auto tileObjects = mapObjectsHandler::Get( ).getAllMapObjects( map.first );

		// Exemplo de tileset (5 linhas × 7 colunas):
		TileSet exemploTileset = {
			{ l,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,t,t,t,t,t,t,t,t,t,t,g,t,t,t,t,t,t,t,t,t,t,r,n,n,n,n,n},
			{ n,n,n,n,n,n,n,n,n,n,l,g,r,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n},
			{ n,n,n,n,n,n,n,n,n,n,l,g,r,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n},
			{ n,n,n,n,n,n,n,n,n,n,l,g,r,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n},
			{ n,n,n,n,n,n,n,n,n,n,l,g,r,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n},
			{ n,n,n,n,n,n,n,n,n,n,l,g,r,n,n,n,n,n,n,n,n,n,n,n,n,n,n,n},
			{ l,t,t,t,t,t,t,t,t,t,t,d,t,t,t,t,t,t,t,t,t,t,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r,n,n,n,n,n},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,t,t,t,t,t,r},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,r},
			{ l,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,t,t,t,t,t,r},
			{ l,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,t,r,n,n,n,n,n},
		};
		TileSet correctedTileSet = corrigirTilesetPorCelulas( exemploTileset , tileObjects );
		addMapTileset( map.first , exemploTileset );
	}


	return true; // All map objects initialized successfully
}