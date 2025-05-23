#include "LevelManager.h"
#include <cstdlib>
#include <ctime>

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
	bool hasEnemyToRespawn = false;

	bool canRespawn = this->respawnTimer > levels[ currentLevelIndex ].RespawnTimer;

	for ( auto & enemy : enemies ) {
		enemy->updateEntity( );
		if ( !enemy->isAlive( ) && this->respawnCount < levels[ currentLevelIndex ].RespawnCount ) {
			hasEnemyToRespawn = true;
			if ( canRespawn ) {
				enemy->Respawn( );
				this->respawnTimer = 0;
				canRespawn = false;
				this->respawnCount++;
			}
		}
	}

	if ( hasEnemyToRespawn ) {
		this->respawnTimer++;
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
		return GVector2D( 0 , 0 );
	}

	// Obtém a posição atual do jogador
	GVector2D playerPos = player->getEntityPosition( );

	// Define um raio ao redor do jogador para spawn aleatório (ajuste conforme necessário)
	const float spawnRadius = 300.f;

	// Gera um ângulo e distância aleatórios dentro do raio
	float angle = static_cast< float >( rand( ) ) / RAND_MAX * 2.0f * 3.14159265f;
	float distance = static_cast< float >( rand( ) ) / RAND_MAX * spawnRadius;

	// Calcula as coordenadas x e y ao redor do jogador
	float x = playerPos.x + cosf( angle ) * distance;
	float y = playerPos.y + sinf( angle ) * distance;

	return GVector2D( x , y );
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
		enemies.at( i )->setEntityPosition( GVector2D( i * 128 , i * 128 ) );
		enemies.at( i )->Respawn( );
		i++;
	}
}

std::vector<std::shared_ptr<CEnemyEntity>> LevelManager::getEnemies( ) {
	std::lock_guard<std::mutex> lock( managerMutex );
	return enemies;
}