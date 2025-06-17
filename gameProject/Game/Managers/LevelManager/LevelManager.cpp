#include "LevelManager.h"

#include <random>

#include "../../gameObjects/gameMap/gameMap.h"
#include "../../Handlers/mapObjectsHandler/mapObjectsHandler.h"
#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/trapsHandler/trapsHandler.h"
#include "../../Handlers/attackHandler/attackHandler.h"
#include "../../Handlers/LevelHandler/LevelHandler.h"
#include "../../../Globals/Globals.h"


LevelManager levelManager;

// Estrutura auxiliar para definir as regras de progress�o de um tipo de inimigo
struct EnemyProgression {
	CEnemyType base;
	CEnemyType medium;
	CEnemyType advanced;
};

int getRandom( int min , int max ) {
	static std::mt19937 engine( std::chrono::high_resolution_clock::now( ).time_since_epoch( ).count( ) );
	std::uniform_int_distribution<int> dist( min , max );
	return dist( engine ); // Corrigido para passar o gerador de n�meros aleat�rios 'engine' como argumento para 'dist'.  
}

/**
 * @brief Gera um n�vel proceduralmente, aumentando a dificuldade com base no progresso.
 * * @param type O tipo do mapa para este n�vel.
 * @param name O nome base para o mapa.
 * @param levelIndex O �ndice do n�vel atual (come�ando em 0 para o primeiro n�vel).
 * @param totalLevels O n�mero total de n�veis procedurais (excluindo o chefe).
 * @param baseSpawnCount A quantidade inicial de inimigos por sala.
 * @param maxSpawnCount A quantidade m�xima de inimigos por sala no final da progress�o.
 */
void LevelManager::generateProceduralLevel( mapType type , std::string name , int levelIndex , int totalLevels , int baseSpawnCount , int maxSpawnCount ) {
	DungeonLayout * dungeon = mapObjectsHandler::Get( ).getDungeonLayout( type );
	if ( !dungeon ) {
		// Log de erro ou tratamento para dungeon nulo
		return;
	}

	int numRooms = dungeon->numRooms;
	if ( numRooms == 0 ) return;

	// Define as cadeias de progress�o para cada tipo de inimigo
	EnemyProgression meleeProgression = { CEnemyType::MELEE_ENEMY, CEnemyType::MELEE_ENEMY_MEDIUM, CEnemyType::MELEE_ENEMY_ADVANCED };
	EnemyProgression rangedProgression = { CEnemyType::RANGED_ENEMY, CEnemyType::RANGED_ENEMY_MEDIUM, CEnemyType::RANGED_ENEMY_ADVANCED };

	// Itera sobre cada sala do n�vel para gerar inimigos espec�ficos para ela
	for ( int roomIndex = 0; roomIndex < numRooms; ++roomIndex ) {

		// Calcula o progresso geral do jogador atrav�s de todos os n�veis (de 0.0 a 1.0)
		// Isso determina a dificuldade da sala atual.
		float overallProgress = ( float ) ( levelIndex * numRooms + roomIndex ) / ( float ) ( totalLevels * numRooms );

		LevelData levelData;
		levelData.levelNumber = roomIndex + 1;
		levelData.mapName = name + "_" + std::to_string( roomIndex + 1 );
		levelData.Traps = 0; // Pode ser expandido para gerar armadilhas tamb�m

		std::vector<LevelEnemySpawnerData> spawners;

		// --- L�gica para determinar o tipo de inimigo ---
		// Thresholds para introdu��o de inimigos mais fortes
		const float mediumThreshold = 0.33f;
		const float advancedThreshold = 0.66f;

		// Escolhe o tipo de inimigo Melee
		CEnemyType meleeToSpawn = meleeProgression.base;
		if ( overallProgress > advancedThreshold ) {
			// Chance de spawnar inimigo avan�ado aumenta conforme o progresso
			if ( getRandom( 0 , 100 ) < ( int ) ( ( ( overallProgress - advancedThreshold ) / ( 1.0f - advancedThreshold ) ) * 100.0f ) ) {
				meleeToSpawn = meleeProgression.advanced;
			}
			else {
				meleeToSpawn = meleeProgression.medium;
			}
		}
		else if ( overallProgress > mediumThreshold ) {
			// Chance de spawnar inimigo m�dio aumenta
			if ( getRandom( 0 , 100 ) < ( int ) ( ( ( overallProgress - mediumThreshold ) / ( advancedThreshold - mediumThreshold ) ) * 100.0f ) ) {
				meleeToSpawn = meleeProgression.medium;
			}
			else {
				meleeToSpawn = meleeProgression.base;
			}
		}

		// Escolhe o tipo de inimigo Ranged (l�gica similar)
		CEnemyType rangedToSpawn = rangedProgression.base;
		if ( overallProgress > advancedThreshold ) {
			if ( getRandom( 0 , 100 ) < ( int ) ( ( ( overallProgress - advancedThreshold ) / ( 1.0f - advancedThreshold ) ) * 100.0f ) ) {
				rangedToSpawn = rangedProgression.advanced;
			}
			else {
				rangedToSpawn = rangedProgression.medium;
			}
		}
		else if ( overallProgress > mediumThreshold ) {
			if ( getRandom( 0 , 100 ) < ( int ) ( ( ( overallProgress - mediumThreshold ) / ( advancedThreshold - mediumThreshold ) ) * 100.0f ) ) {
				rangedToSpawn = rangedProgression.medium;
			}
			else {
				rangedToSpawn = rangedProgression.base;
			}
		}

		// --- L�gica para determinar a quantidade de inimigos ---
		int currentSpawnCount = baseSpawnCount + ( int ) ( ( maxSpawnCount - baseSpawnCount ) * overallProgress );

		// Adiciona os spawners para a sala atual
		spawners.push_back( { meleeToSpawn, getRandom( currentSpawnCount / 2, currentSpawnCount ), 10, 5, 0, 0.0 } );
		spawners.push_back( { rangedToSpawn, getRandom( currentSpawnCount / 2, currentSpawnCount ), 10, 5, 0, 0.0 } );

		// Adiciona os spawners definidos aos dados do n�vel
		for ( const auto & spawner : spawners ) {
			if ( spawner.SpawnCount > 0 ) { // S� adiciona se houver inimigos para spawnar
				levelData.enemySpawners.emplace( spawner.type , spawner );
			}
		}

		// Cria o objeto da sala e o adiciona ao handler
		gameRoomLevel gameRoom( levelData , roomIndex + 1 );
		LevelHandler::Get( ).addLevel( type , gameRoom );
	}
}


void LevelManager::generateBossLevel( ) {
	// A gera��o do chefe pode permanecer mais est�tica, pois � uma sala �nica e especial.
	// Pode incluir inimigos avan�ados como minions.
	std::vector<LevelEnemySpawnerData> spawnData = {
		{ CEnemyType::MELEE_ENEMY_ADVANCED, 5, 10, 8, 0, 0.0 },
		{ CEnemyType::RANGED_ENEMY_ADVANCED, 5, 10, 8, 0, 0.0 },
		// { CEnemyType::BOSS_TYPE, 1, 0, 0, 0, 0.0 } // Exemplo de spawner para o chefe
	};

	DungeonLayout * dungeon = mapObjectsHandler::Get( ).getDungeonLayout( bossMap );
	if ( !dungeon || dungeon->numRooms == 0 ) return;

	LevelData levelData;
	levelData.levelNumber = 1; // Apenas uma sala
	levelData.mapName = "bossMap_1";

	for ( const auto & spawner : spawnData ) {
		levelData.enemySpawners.emplace( spawner.type , spawner );
	}

	gameRoomLevel gameRoom( levelData , 1 );
	LevelHandler::Get( ).addLevel( bossMap , gameRoom );
}


void LevelManager::generateLevels( ) {
	// Gera os 3 n�veis com dificuldade progressiva
	const int totalProceduralLevels = 3;
	generateProceduralLevel( firstMap , "firstMap" , 0 , totalProceduralLevels , 2 , 5 );
	generateProceduralLevel( secondMap , "secondMap" , 1 , totalProceduralLevels , 3 , 7 );
	generateProceduralLevel( thirdMap , "thirdMap" , 2 , totalProceduralLevels , 5 , 10 );

	// Gera o n�vel do chefe separadamente
	generateBossLevel( );
}

void LevelManager::restartLevels( ) {

	this->started = false;
	this->currentLevel = 0;
	this->mapLevels.clear( );
	LevelHandler::Get( ).clearLevels( );
	entitiesHandler::Get( ).clearSpawnedEntities( );
	trapsHandler::Get( ).clearSpawnedTraps( );
	attackHandler::Get( ).clearAttacks( );
}

void LevelManager::initializeLevels( ) {
	CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );

	if ( localPlayer != nullptr ) {
		localPlayer->Respawn( );
	}

	for ( int i = 0; i <= mapType::bossMap; ++i ) {
		mapType levelMap = static_cast< mapType >( i );
		std::vector<gameRoomLevel> * roomLevels = LevelHandler::Get( ).getMapRoomLevels( levelMap );
		if ( roomLevels == nullptr ) {
			continue;
		}
		for ( auto & room : *roomLevels ) {
			this->mapLevels[ levelMap ].emplace( std::make_pair( room.getRoomID( ) , room ) );
		}
	}
}

void LevelManager::initialize( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	if ( started ) {
		restartLevels( ); // Reinicializa os n�veis se j� estiverem iniciados
	}

	generateLevels( );
	initializeLevels( );
}

void LevelManager::update( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	if ( !started )
		return;

	mapType map = static_cast< mapType >( this->currentLevel );
	Globals::Get( ).getGame( )->setCurrentGameLevel( this->currentLevel );

	auto it = this->mapLevels.find( map );
	if ( it != this->mapLevels.end( ) ) {
		for ( auto & roomPair : it->second ) {
			roomPair.second.update( );
		}
	}
}

void LevelManager::start( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );

	this->currentLevel = 0;
	started = true;

	mapType map = static_cast< mapType >( this->currentLevel );
	auto it = this->mapLevels.find( map );
	if ( it != this->mapLevels.end( ) ) {
		gameMap::Get( ).setMap( map );
		for ( auto & roomPair : it->second ) {
			roomPair.second.init( );
		}
	}
}

int LevelManager::roomsInCurrentLevel( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	mapType map = static_cast< mapType >( this->currentLevel );
	auto it = this->mapLevels.find( map );
	if ( it != this->mapLevels.end( ) ) {
		return it->second.size( );
	}
	return 0;
}

void LevelManager::moveToNextLevel( ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	if ( this->currentLevel >= mapType::bossMap ) {
		return; // J� est� no �ltimo n�vel
	}

	this->currentLevel++;
	mapType map = static_cast< mapType >( this->currentLevel );
	auto it = this->mapLevels.find( map );
	if ( it != this->mapLevels.end( ) ) {
		gameMap::Get( ).setMap( map );
		for ( auto & roomPair : it->second ) {
			roomPair.second.init( );
		}
	}
}

bool LevelManager::hasAliveEnemyOnCurrentRoom( int roomID ) {
	std::lock_guard<std::mutex> lock( this->m_mutex );
	mapType map = static_cast< mapType >( this->currentLevel );
	auto mapIt = this->mapLevels.find( map );
	if ( mapIt != this->mapLevels.end( ) ) {
		auto roomIt = mapIt->second.find( roomID );
		if ( roomIt != mapIt->second.end( ) ) {
			return roomIt->second.hasEnemyAlive( );
		}
	}
	return false;
}
