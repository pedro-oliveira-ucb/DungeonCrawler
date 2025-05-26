#include "LevelManager.h"
#include <cstdlib>
#include <ctime>
#include <random>


#include "../../../Globals/Globals.h"

#include "../../gameObjects/entitiesHandler/entitiesHandler.h"

LevelManager::LevelManager( ) {

	// No início do main() ou construtor global:
	srand( static_cast< unsigned int >( time( nullptr ) ) );
	loadLevels( );
}

LevelManager levelManager;

void LevelManager::loadLevels( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	levels.push_back( { 1, "Introduction", 10, 10 } );
	levels.push_back( { 2, "Do you dare?", 10, 15 } );
	levels.push_back( { 3, "Nightmare", 20, 20 } );
}


LevelData LevelManager::getCurrentLevel( ) {
	return levels[ currentLevelIndex ];
}

int RandomNumber( int min , int max )
{
	std::random_device rd;
	std::mt19937 rng( rd( ) );
	std::uniform_int_distribution<int> uni( min , max );
	return uni( rng );
}


void LevelManager::moveToNextLevel( ) {
	if ( !isLastLevel( ) ) {
		++currentLevelIndex;
		this->respawnCount = 0;
		spawnEnemiesForLevel( levels[ currentLevelIndex ] );
	}
}

bool LevelManager::hasEnemyAlive( ) {
	for ( const auto & enemy : enemies ) {
		if ( enemy->isAlive( ) && this->respawnCount > levels[ currentLevelIndex ].RespawnCount ) {
			return true;
		}
	}
	return false;
}

bool LevelManager::isLastLevel( ) {

	return currentLevelIndex >= static_cast< int >( levels.size( ) ) - 1;
}

void LevelManager::updateEnemies( ) {


	auto lastUse = this->respawnTimer;
	// Calcula delta time em segundos
	std::chrono::duration<float> delta = now - lastUse;

	bool canRespawn = delta.count( ) >= levels[ currentLevelIndex ].RespawnTimer;

	for ( auto & enemy : enemies ) {
		enemy->updateEntity( );
		if ( !enemy->isAlive( ) && this->respawnCount < levels[ currentLevelIndex ].RespawnCount ) {
			if ( canRespawn ) {
				enemy->Respawn( );
				this->respawnTimer = now;
				canRespawn = false;
				this->respawnCount++;
			}
		}
	}
}

void LevelManager::updateLevel( )
{
	std::lock_guard<std::mutex> lock( managerMutex );
	if ( !started ) {
		spawnEnemiesForLevel( levels[ currentLevelIndex ] );
		started = true;
	}

	if ( !hasEnemyAlive( ) ) {
		moveToNextLevel( );
	}

	updateEnemies( );
}

GVector2D getRandomAreaAroundLocalPlayer( ) {

	auto player = entitiesHandler::Get( ).getLocalPlayer( );

	if ( player == nullptr ) {
		return GVector2D( 0, 0 );
	}
	GVector2D localPos = player->getEntityPosition( );

	int RandomX = RandomNumber( -850 , 850 );
	int RandomY = RandomNumber( -850 , 850 );

	return GVector2D( localPos.x + RandomX , localPos.y + RandomY );
}


void LevelManager::spawnEnemiesForLevel( const LevelData & data ) {
	auto enemyList = entitiesHandler::Get( ).getEnemies( );
	for ( int i = 0; i < data.enemyCount; i++ ) {
		CEnemyEntity * rawEnemyPtr = enemyList.at( 0 );
		std::shared_ptr<CEnemyEntity> enemy( rawEnemyPtr->Clone( ) );
		entitiesHandler::Get( ).addSpawnedEntity( enemy.get( ) );
		enemies.push_back( enemy );
	}
	
	int i = 0;
	for ( int i = 0; i < enemies.size( ); i++ ) {
		
		enemies.at( i )->setEntityPosition( getRandomAreaAroundLocalPlayer() );
		enemies.at( i )->Respawn( );
		i++;
	}
}

std::vector<std::shared_ptr<CEnemyEntity>> LevelManager::getEnemies( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	return enemies;
}