#include "LevelManager.h"
#include <cstdlib>
#include <ctime>

#include "../../../Utils/utils.h"
#include "../../../Globals/Globals.h"

#include "../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../collisionManager/collisionManager.h"

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

GVector2D getRandomAreaAroundLocalPlayer( CBaseEntity * ent ) {

	auto player = entitiesHandler::Get( ).getLocalPlayer( );

	if ( player == nullptr ) {
		return GVector2D( 0 , 0 );
	}
	GVector2D localPos = player->getEntityPosition( );

	bool okayPlace = false;
	GVector2D deriserdSpawnPlace;

	while ( !okayPlace ) {
		int RandomX = utils::Get( ).randomNumber( -850 , 850 );
		int RandomY = utils::Get( ).randomNumber( -850 , 850 );
		deriserdSpawnPlace = GVector2D( localPos.x + RandomX , localPos.y + RandomY );
		if ( CollisionManager::Get( ).isSpaceFree( deriserdSpawnPlace , GVector2D( 150 , 150 ) ) ) {
			okayPlace = true;
		}
	}

	return deriserdSpawnPlace;
}

void LevelManager::spawnEnemiesForLevel( const LevelData & data ) {
	auto enemyList = entitiesHandler::Get( ).getEnemies( );

	for ( int i = 0; i < data.enemyCount; i++ ) {
		// Cria o clone como unique_ptr
		std::unique_ptr<CEnemyEntity> enemy = enemyList->at( CEnemyType::MELEE_ENEMY )->uniqueClone();

		enemies.push_back( enemy.get() );

		// Adiciona aos handlers (precisa passar ponteiro do unique_ptr para addSpawnedEnemy)
		entitiesHandler::Get( ).addSpawnedEnemy( &enemy ); // passa ponteiro para o unique_ptr
	}

	// Inicialização das posições
	for ( auto & e : enemies ) {
		e->setEntityPosition( getRandomAreaAroundLocalPlayer( e ) );
		e->Respawn( );
	}
}
