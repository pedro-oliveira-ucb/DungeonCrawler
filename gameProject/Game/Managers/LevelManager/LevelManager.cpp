#include "LevelManager.h"
#include <cstdlib>
#include <ctime>

#include "../../../Utils/utils.h"
#include "../../../Globals/Globals.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/trapsHandler/trapsHandler.h"
#include "../collisionManager/collisionManager.h"

LevelManager::LevelManager( ) {
	loadLevels( );
}

LevelManager levelManager;


void LevelManager::generateLevel1( ) {

	std::lock_guard<std::mutex> lock( managerMutex );
	//LEVEL 1
	LevelEnemySpawnerData basicMeleeEnemySpawner;
	basicMeleeEnemySpawner.type = CEnemyType::MELEE_ENEMY;
	basicMeleeEnemySpawner.SpawnCount = 10;
	basicMeleeEnemySpawner.RespawnCount = 5;
	basicMeleeEnemySpawner.RespawnTimer = 5;
	basicMeleeEnemySpawner.timeSinceLastRespawn = now;

	LevelEnemySpawnerData mediumMeleeEnemySpawner;
	mediumMeleeEnemySpawner.type = CEnemyType::MELEE_ENEMY_MEDIUM;
	mediumMeleeEnemySpawner.SpawnCount = 5;
	mediumMeleeEnemySpawner.RespawnCount = 5;
	mediumMeleeEnemySpawner.RespawnTimer = 10;
	mediumMeleeEnemySpawner.timeSinceLastRespawn = now;

	LevelData level;
	level.levelNumber = 1;
	level.mapName = "Map1";
	level.Traps = 10;
	level.enemySpawners.push_back( basicMeleeEnemySpawner );
	level.enemySpawners.push_back( mediumMeleeEnemySpawner );
	levels.push_back( level );
}

void LevelManager::generateLevel2( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	//LEVEL 1
	LevelEnemySpawnerData basicMeleeEnemySpawner;
	basicMeleeEnemySpawner.type = CEnemyType::MELEE_ENEMY;
	basicMeleeEnemySpawner.SpawnCount = 10;
	basicMeleeEnemySpawner.RespawnCount = 5;
	basicMeleeEnemySpawner.RespawnTimer = 5;
	basicMeleeEnemySpawner.timeSinceLastRespawn = now;

	LevelEnemySpawnerData mediumMeleeEnemySpawner;
	mediumMeleeEnemySpawner.type = CEnemyType::MELEE_ENEMY_MEDIUM;
	mediumMeleeEnemySpawner.SpawnCount = 5;
	mediumMeleeEnemySpawner.RespawnCount = 5;
	mediumMeleeEnemySpawner.RespawnTimer = 10;
	mediumMeleeEnemySpawner.timeSinceLastRespawn = now;

	LevelData level;
	level.levelNumber = 1;
	level.mapName = "Map2";
	level.Traps = 10;
	level.enemySpawners.push_back( basicMeleeEnemySpawner );
	level.enemySpawners.push_back( mediumMeleeEnemySpawner );
	levels.push_back( level );
}


void LevelManager::loadLevels( ) {
	generateLevel1( );
	generateLevel2( );
}

LevelData LevelManager::getCurrentLevel( ) {
	return levels[ currentLevelIndex ];
}

void LevelManager::moveToNextLevel( ) {
	if ( !isLastLevel( ) ) {
		++currentLevelIndex;
		this->respawnCount = 0;
		spawnEnemiesForLevel( levels[ currentLevelIndex ] );
	}
}

bool LevelManager::hasEnemyAlive( ) {
	bool foundAliveEnemy = false;
	bool canRespawn = false;

	for ( auto it = enemies.begin( ); it != enemies.end( );) {
		for ( auto enemies : it->second ) {
			if ( enemies->isAlive( ) ) {
				foundAliveEnemy = true;
			}
			++enemies;
		}
		if ( this->levels[ currentLevelIndex ].enemySpawners.at( it->first ).respawnedCount <
			this->levels[ currentLevelIndex ].enemySpawners.at( it->first ).RespawnCount ) {
			canRespawn = true;
		}
		++it; // Move to the next enemy
	}

	return foundAliveEnemy || canRespawn;
}

bool LevelManager::isLastLevel( ) {
	return currentLevelIndex >= static_cast< int >( levels.size( ) ) - 1;
}

bool LevelManager::canRespawn( CEnemyType enemyType ) {
	auto & currentLevel = levels[ currentLevelIndex ];
	auto & enemySpawnHandler = currentLevel.enemySpawners.at( enemyType );

	bool canRespawn = false;

	if ( enemySpawnHandler.respawnedCount < enemySpawnHandler.RespawnCount ) {
		auto lastUse = enemySpawnHandler.timeSinceLastRespawn;
		std::chrono::duration<float> delta = now - lastUse;
		if ( delta.count( ) >= enemySpawnHandler.RespawnTimer ) {
			canRespawn = true;
		}
	}

	return canRespawn;
}


void LevelManager::updateEnemies( ) {

	// Calcula delta time em segundos
	auto & currentLevel = levels[ currentLevelIndex ];

	for ( auto it = enemies.begin( ); it != enemies.end( ); ++it ) {
		auto & enemySpawnHandler = currentLevel.enemySpawners.at( it->first );
		bool respawnEnemy = canRespawn( it->first );
		if ( respawnEnemy ) {
			for ( auto & enemy : it->second ) {
				if ( !enemy->isAlive( ) && enemy->deathAnimationFinished( ) ) {
					enemy->setEntityPosition( entitiesHandler::Get( ).getRandomPlaceAroundPlayer( 850 ) );
					enemy->Respawn( );
					enemySpawnHandler.respawnedCount++;
					enemySpawnHandler.timeSinceLastRespawn = now;
					break;
				}
			}
		}
	}
}

void LevelManager::updateLevel( )
{
	std::lock_guard<std::mutex> lock( managerMutex );
	if ( !started ) {
		currentLevelIndex = 0;
		spawnEnemiesForLevel( levels[ currentLevelIndex ] );
		started = true;
	}

	if ( !hasEnemyAlive( ) ) {
		moveToNextLevel( );
	}

	updateEnemies( );
}

void LevelManager::spawnEnemiesForLevel( const LevelData & data ) {
	auto enemyList = entitiesHandler::Get( ).getSpawnableEnemies( );
	for ( auto it = data.enemySpawners.begin( ); it != data.enemySpawners.end( ); ++it ) {
		for ( int i = 0; i < it->SpawnCount; i++ ) {
			std::unique_ptr<CEnemyEntity> enemy = enemyList->at( it->type )->uniqueClone( );
			enemies[ it->type ].push_back( enemy.get( ) );
			entitiesHandler::Get( ).addSpawnedEnemy( &enemy );
		}
	}

	for ( int i = 0; i < data.Traps; i++ ) {
		trapsHandler::Get( ).spawnTrap( TrapType::SPIKES );
	}

	// Inicialização das posições
	for ( auto & e : enemies ) {
		for ( auto & enemy : e.second ) {
			enemy->setEntityPosition( entitiesHandler::Get( ).getRandomPlaceAroundPlayer( 850 ) );
			enemy->Respawn( );
		}
	}
}
