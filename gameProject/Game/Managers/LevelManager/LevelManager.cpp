#include "LevelManager.h"
#include "../../gameObjects/entitiesHandler/entitiesHandler.h"

LevelManager::LevelManager( ) {
	loadLevels( );
}

LevelManager levelManager;

void LevelManager::loadLevels( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	levels.push_back( { 1, "Introduction", 10, 10 } );
	levels.push_back( { 2, "Do you dare?", 10, 15 } );
	levels.push_back( { 3, "Nightmare", 20, 20 } );
}


const LevelData & LevelManager::getCurrentLevel( ) const {
	std::lock_guard<std::mutex> lock( managerMutex );
	return levels[ currentLevelIndex ];
}

void LevelManager::moveToNextLevel( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	if ( !isLastLevel( ) ) {
		++currentLevelIndex;
		this->respawnCount = 0;
		spawnEnemiesForLevel( levels[ currentLevelIndex ] );
	}
}

bool LevelManager::hasEnemyAlive( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	for ( const auto & enemy : enemies ) {
		if ( enemy->isAlive( ) ) {
			return true;
		}
	}
	return false;
}

bool LevelManager::isLastLevel( ) const {
	std::lock_guard<std::mutex> lock( managerMutex );
	return currentLevelIndex >= static_cast< int >( levels.size( ) ) - 1;
}

void LevelManager::updateEnemies( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	for ( auto & enemy : enemies ) {
		if ( enemy->isAlive( ) ) {
			enemy->updateEntity( );
		}
		else if ( this->respawnCount < levels[ currentLevelIndex ].RespawnCount ) {
			enemy->Respawn( );
			this->respawnCount++;
		}
	}
}

void LevelManager::updateLevel( )
{
	if ( !started ) {
		spawnEnemiesForLevel( levels[ currentLevelIndex ] );
		started = true;
	}

	if ( !hasEnemyAlive( ) ) {
		moveToNextLevel( );
	}

	updateEnemies( );
}


void LevelManager::spawnEnemiesForLevel( const LevelData & data ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	auto enemyList = entitiesHandler::Get( ).getEnemies( );
	for ( int i = 0; i < data.enemyCount; i++ ) {
		CEnemyEntity * rawEnemyPtr = enemyList.at( i % 2 );
		std::shared_ptr<CEnemyEntity> enemy( rawEnemyPtr );
		enemies.push_back( enemy );
	}
	for ( auto & enemy : enemyList ) {
		enemy->Respawn( );
	}
}

std::vector<std::shared_ptr<CEnemyEntity>> LevelManager::getEnemies( ) const {
	std::lock_guard<std::mutex> lock( managerMutex );
	return enemies;
}