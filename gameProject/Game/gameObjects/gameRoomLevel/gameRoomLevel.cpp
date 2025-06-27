#include "gameRoomLevel.h"


#include "../gameMap/gameMap.h"
#include "../../Managers/collisionManager/collisionManager.h"
#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/trapsHandler/trapsHandler.h"

#include "../../../Utils/Log/Log.h"
#include "../../../Utils/utils.h"
#include "../../../Globals/Globals.h"

gameRoomLevel::gameRoomLevel( const gameRoomLevel & other )
{
	this->RoomID = other.RoomID;
	this->levelData = other.levelData;
	this->initialized = other.initialized;
}

void gameRoomLevel::init( ) {
	// Initialize the game room level
	if ( !initialized ) {
		spawnEnemies( );
		spawnTraps( );
		initialized = true;
	}
}

void gameRoomLevel::restart( ) {
	this->respawnEnemies( true ); // Force respawn all enemies
}

void gameRoomLevel::update( ) {
	updateEntities( );
}

bool gameRoomLevel::hasEnemyAlive( )
{
	std::lock_guard<std::mutex> lock( this->m_mutex );

	if ( this->enemies.empty( ) ) {
		return false; // No enemies to check
	}

	for ( const auto & enemyType : this->enemies ) {
		if ( this->levelData.enemySpawners.find( enemyType.first ) == this->levelData.enemySpawners.end( ) ) {
			continue;
		}

		for ( const auto & enemy : enemyType.second ) {
			if ( enemy.second->isAlive( ) ) {
				return true; // Found an alive enemy
			}
		}

		if ( this->levelData.enemySpawners[ enemyType.first ].respawnedCount < this->levelData.enemySpawners[ enemyType.first ].RespawnCount ) {
			return true; // There are enemies that can respawn
		}
	}


	return false;
}

void gameRoomLevel::updateEntities( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	if ( this->enemies.empty( ) ) {
		return;
	}

	GVector2D localPosition = Globals::Get( ).getGame( )->getCurrentLocalPlayerPosition( );


	int currentPlayerID = gameMap::Get( ).getRoomIdAtPosition( localPosition );

	bool followPlayer = this->RoomID == currentPlayerID;

	if ( std::fabs( this->RoomID - currentPlayerID ) > 1 )
		return;

	for ( auto & enemyType : this->enemies ) {

		auto enemySpawner = this->levelData.enemySpawners.find( enemyType.first );

		if ( enemySpawner == this->levelData.enemySpawners.end( ) ) {
			continue;
		}

		for ( auto it = this->enemies.begin( ); it != this->enemies.end( ); ++it ) {
			for ( auto enemy = it->second.begin( ); enemy != it->second.end( ); ++enemy ) {
			/*	if ( enemy->second->getEntityPosition( ).distTo( localPosition ) > 150 )
					continue;*/

				if ( !enemy->second->isAlive( ) && enemy->second->deathAnimationFinished( ) ) {
					if ( canRespawn( enemyType.first ) ) {
						GVector2D spawnPosition = getValidPositionOnCurrentRoom( );
						enemy->second->setEntityPosition( spawnPosition );
						enemy->second->Respawn( );
						enemySpawner->second.respawnedCount++;
						Log::Print( "Respawning enemy of roomID %d" , this->RoomID );
					}
					continue; // Skip to the next enemy if this one is dead and respawned
				}

				entitiesHandler::Get( ).updateSpawnedEnemies( enemy->first , followPlayer );
			}
		}
	}
}

void gameRoomLevel::spawnEnemies( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	std::unordered_map<CEnemyType , std::unique_ptr<CEnemyEntity>> * availableSpawnableEnemies = entitiesHandler::Get( ).getSpawnableEnemies( );
	std::vector<std::unique_ptr<CEnemyEntity>> * spawnedEntities = entitiesHandler::Get( ).getSpawnedEnemies( );
	
	entitiesHandler::Get( ).updateEnemiesCollision( );

	for ( auto it = availableSpawnableEnemies->begin( ); it != availableSpawnableEnemies->end( ); ++it ) {

		auto & spawner = this->levelData.enemySpawners[ it->first ];
		int currentIndex = spawnedEntities->size( );
		for ( int i = 0; i < spawner.SpawnCount; i++ ) {
			std::unique_ptr<CEnemyEntity> spawnedEnemy = it->second->uniqueClone( );

			GVector2D spawnPosition = getValidPositionOnCurrentRoom( );
			spawnedEnemy->setEntityPosition( spawnPosition );

			CEnemyEntity * spawnedEnemyRawPointer = spawnedEnemy.get( );

			enemies[ it->first ][ currentIndex ] = spawnedEnemyRawPointer;

			entitiesHandler::Get( ).addSpawnedEnemy( std::move( spawnedEnemy ) );
			currentIndex++;
		}
	}
}

bool gameRoomLevel::canRespawn( CEnemyType enemyType ) {
	// Esta função é chamada por updateEnemies, que já tem lock.
	auto & currentLevelData = this->levelData; // Renomeado para clareza
	auto spawnerIt = currentLevelData.enemySpawners.find( enemyType ); // Evitar .at() que lança exceção

	if ( spawnerIt == currentLevelData.enemySpawners.end( ) ) {
		return false; // Tipo de inimigo não tem dados de spawner para este nível
	}
	auto & enemySpawnData = spawnerIt->second; // Renomeado para clareza

	bool canRespawnFlag = false; // Renomeado para clareza

	double currentGameTime = Globals::Get( ).getGame( )->getCurrentGameTime( );

	if ( enemySpawnData.respawnedCount < enemySpawnData.RespawnCount ) {
		double delta = currentGameTime - enemySpawnData.lastRespawnTime;
		if ( delta >= enemySpawnData.RespawnTimer ) {
			canRespawnFlag = true;
		}
	}

	return canRespawnFlag;
}

GVector2D gameRoomLevel::getValidPositionOnCurrentRoom( ) {
	GVector2D position = GVector2D( 0 , 0 );

	bool validPosition = false;
	// Ensure the spawn position is valid

	int maxTries = 10;
	int tryCount = 0;

	do {
		position = gameMap::Get( ).getRandomPlace( this->RoomID );
		if ( CollisionManager::Get( ).isSpaceFree( position , GVector2D( 150 , 150 ) ) ) {
			validPosition = true;
		}
		tryCount++;
	} while ( !validPosition && tryCount < maxTries );

	return position;
}

void gameRoomLevel::respawnEnemies( bool forceRespawn ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	if ( this->enemies.empty( ) ) {
		return;
	}
	for ( auto it = this->enemies.begin( ); it != this->enemies.end( ); ++it ) {
		for ( auto enemy = it->second.begin( ); enemy != it->second.end( ); ++enemy ) {
			if ( !enemy->second->isAlive( ) ) {
				if ( !forceRespawn && !canRespawn( it->first ) ) {
					continue; // Não pode ressuscitar ainda
				}
				GVector2D spawnPosition = getValidPositionOnCurrentRoom( );
				enemy->second->setEntityPosition( spawnPosition );
				enemy->second->Respawn( );
			}
		}
	}
}

void gameRoomLevel::spawnTraps( ) {
	for ( int i = 0; i < this->levelData.Traps; i++ ) {
		GVector2D spawnPosition = getValidPositionOnCurrentRoom( );
		if ( utils::Get( ).onProbability( 50 ) ) {
			trapsHandler::Get( ).spawnTrap( TrapType::SPIKES , spawnPosition );
		}
		else {
			trapsHandler::Get( ).spawnTrap( TrapType::EXPLOSIVE , spawnPosition );
		}
	}
}

