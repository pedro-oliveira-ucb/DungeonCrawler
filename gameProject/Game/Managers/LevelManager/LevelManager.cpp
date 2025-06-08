#include "LevelManager.h"

#include "../../gameObjects/gameMap/gameMap.h"
#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"
#include "../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../Handlers/LevelHandler/LevelHandler.h"

LevelManager levelManager;

void LevelManager::generateFirstLevel( ) {

	mapType levelMap = firstMap;

	DungeonLayout * dungeon = mapObjectsHandler::Get( ).getDungeonLayout( levelMap );
	if ( dungeon == nullptr ) {
		return;
	}

	int numRooms = dungeon->numRooms;

	for ( int i = 1; i <= numRooms; ++i ) {

		LevelEnemySpawnerData basicEnemySpawnerData = {
			CEnemyType::MELEE_ENEMY , 5 , 3 , 10 , 0 , 0.0
		};
		LevelData levelData = {
			1 , "firstMap_" + std::to_string( i ) , 0 , { { CEnemyType::MELEE_ENEMY , basicEnemySpawnerData } }
		};
		gameRoomLevel gameRoom( levelData , i );

		// Adiciona o LevelData ao LevelHandler
		LevelHandler::Get( ).addLevel( levelMap , gameRoom );
	}
}

void LevelManager::generateSecondLevel( ) {

	mapType levelMap = secondMap;

	DungeonLayout * dungeon = mapObjectsHandler::Get( ).getDungeonLayout( levelMap );
	if ( dungeon == nullptr ) {
		return;
	}

	int numRooms = dungeon->numRooms;

	for ( int i = 1; i <= numRooms; ++i ) {

		LevelEnemySpawnerData basicEnemySpawnerData = {
			CEnemyType::MELEE_ENEMY , 5 , 3 , 10 , 0 , 0.0
		};
		LevelData levelData = {
			1 , "secondMap_" + std::to_string( i ) , 0 , { { CEnemyType::MELEE_ENEMY , basicEnemySpawnerData } }
		};
		gameRoomLevel gameRoom( levelData , i );

		// Adiciona o LevelData ao LevelHandler
		LevelHandler::Get( ).addLevel( levelMap , gameRoom );
	}
}

void LevelManager::generateThirdLevel( ) {

	mapType levelMap = thirdMap;

	DungeonLayout * dungeon = mapObjectsHandler::Get( ).getDungeonLayout( levelMap );
	if ( dungeon == nullptr ) {
		return;
	}

	int numRooms = dungeon->numRooms;

	for ( int i = 1; i <= numRooms; ++i ) {

		LevelEnemySpawnerData basicEnemySpawnerData = {
			CEnemyType::MELEE_ENEMY , 5 , 3 , 10 , 0 , 0.0
		};
		LevelData levelData = {
			1 , "secondMap_" + std::to_string( i ) , 0 , { { CEnemyType::MELEE_ENEMY , basicEnemySpawnerData } }
		};
		gameRoomLevel gameRoom( levelData , i );

		// Adiciona o LevelData ao LevelHandler
		LevelHandler::Get( ).addLevel( levelMap , gameRoom );
	}
}

void LevelManager::generateBossLevel( ) {

	mapType levelMap = bossMap;

	DungeonLayout * dungeon = mapObjectsHandler::Get( ).getDungeonLayout( levelMap );
	if ( dungeon == nullptr ) {
		return;
	}

	int numRooms = dungeon->numRooms;

	for ( int i = 1; i <= numRooms; ++i ) {

		LevelEnemySpawnerData basicEnemySpawnerData = {
			CEnemyType::MELEE_ENEMY , 5 , 3 , 10 , 0 , 0.0
		};
		LevelData levelData = {
			1 , "secondMap_" + std::to_string( i ) , 0 , { { CEnemyType::MELEE_ENEMY , basicEnemySpawnerData } }
		};
		gameRoomLevel gameRoom( levelData , i );

		// Adiciona o LevelData ao LevelHandler
		LevelHandler::Get( ).addLevel( levelMap , gameRoom );
	}
}

void LevelManager::generateLevels( )
{
	generateFirstLevel( );
	generateSecondLevel( );
	generateThirdLevel( );
	generateBossLevel( );
}
void LevelManager::initializeLevels( ) {

	for ( int i = 0; i <= mapType::bossMap; ++i ) {
		mapType levelMap = static_cast<mapType>( i );
		std::vector<gameRoomLevel> * roomLevels = LevelHandler::Get( ).getMapRoomLevels( levelMap );
		if ( roomLevels == nullptr ) {
			continue;
		}
		for( auto & room : *roomLevels ) {
			this->mapLevels[ levelMap ].emplace( std::make_pair( room.getRoomID( ) , room ) );
		}
	}
}

void LevelManager::initialize( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	generateLevels( );
	initializeLevels( );
}

void LevelManager::update( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	mapType map = static_cast< mapType >( this->currentLevel );

	if ( this->mapLevels.find( map ) != this->mapLevels.end( ) ) {
		for ( auto & map : this->mapLevels.at( map ) ) {
			map.second.update();
		}
	}
	else {
		// Handle case where map is not found, e.g., log an error or throw an exception
	}
}

void LevelManager::start( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );

	if ( !started ) {
		this->currentLevel = 0;
		started = true;
	}

	mapType map = static_cast< mapType >( this->currentLevel );

	if(this->mapLevels.find(map ) != this->mapLevels.end( )) {
		gameMap::Get( ).setMap( map );

		for ( auto & map : this->mapLevels.at( map )) {
			map.second.init( );
		}		
	} else {
		// Handle case where map is not found, e.g., log an error or throw an exception
	}
}

void LevelManager::moveToNextLevel( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );
	if ( player == nullptr ) {
		return;
	}

	if ( this->currentLevel >= mapType::bossMap ) {
		// Handle case where the player is already at the last level, e.g., end the game or show a message
		return;
	}
	this->currentLevel++;

	mapType map = static_cast< mapType >( this->currentLevel );

	if(this->mapLevels.find(map ) != this->mapLevels.end( )) {
		for ( auto & map : this->mapLevels.at( map ) ) {
			map.second.init( );
		}
		gameMap::Get( ).setMap( map );
	}
}

bool LevelManager::hasAliveEnemyOnCurrentRoom(int roomID )
{
	std::lock_guard<std::mutex> lock( this->m_mutex );

	mapType map = static_cast< mapType >( this->currentLevel );

	if( this->mapLevels.find( map ) != this->mapLevels.end( ) ) {
		if(this->mapLevels.at(map ).find( roomID) != this->mapLevels.at( map ).end( )) {
			gameRoomLevel & room = this->mapLevels.at( map ).at( roomID );
			return room.hasEnemyAlive( );
		}
	}
	
	return false;
}





