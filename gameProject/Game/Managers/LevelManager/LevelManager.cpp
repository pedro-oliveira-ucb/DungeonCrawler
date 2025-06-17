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

// Estrutura auxiliar para definir as regras de progressão de um tipo de inimigo
struct EnemyProgression {
	CEnemyType base;
	CEnemyType medium;
	CEnemyType advanced;
};

int getRandom( int min , int max ) {
	static std::mt19937 engine( std::chrono::high_resolution_clock::now( ).time_since_epoch( ).count( ) );
	std::uniform_int_distribution<int> dist( min , max );
	return dist( engine ); // Corrigido para passar o gerador de números aleatórios 'engine' como argumento para 'dist'.  
}

/**
 * @brief Gera um nível proceduralmente, aumentando a dificuldade com base no progresso.
 * * @param type O tipo do mapa para este nível.
 * @param name O nome base para o mapa.
 * @param levelIndex O índice do nível atual (começando em 0 para o primeiro nível).
 * @param totalLevels O número total de níveis procedurais (excluindo o chefe).
 * @param baseSpawnCount A quantidade inicial de inimigos por sala.
 * @param maxSpawnCount A quantidade máxima de inimigos por sala no final da progressão.
 */
void LevelManager::generateProceduralLevel( mapType type , std::string name , int levelIndex , int totalLevels , int baseSpawnCount , int maxSpawnCount ) {
	DungeonLayout * dungeon = mapObjectsHandler::Get( ).getDungeonLayout( type );
	if ( !dungeon ) {
		// Log de erro ou tratamento para dungeon nulo
		return;
	}

	int numRooms = dungeon->numRooms;
	if ( numRooms == 0 ) return;

	// Define as cadeias de progressão para cada tipo de inimigo
	EnemyProgression meleeProgression = { CEnemyType::MELEE_ENEMY, CEnemyType::MELEE_ENEMY_MEDIUM, CEnemyType::MELEE_ENEMY_ADVANCED };
	EnemyProgression rangedProgression = { CEnemyType::RANGED_ENEMY, CEnemyType::RANGED_ENEMY_MEDIUM, CEnemyType::RANGED_ENEMY_ADVANCED };

	// Itera sobre cada sala do nível para gerar inimigos específicos para ela
	for ( int roomIndex = 0; roomIndex < numRooms; ++roomIndex ) {

		// Calcula o progresso geral do jogador através de todos os níveis (de 0.0 a 1.0)
		// Isso determina a dificuldade da sala atual.
		float overallProgress = ( float ) ( levelIndex * numRooms + roomIndex ) / ( float ) ( totalLevels * numRooms );

		LevelData levelData;
		levelData.levelNumber = roomIndex + 1;
		levelData.mapName = name + "_" + std::to_string( roomIndex + 1 );
		levelData.Traps = 0; // Pode ser expandido para gerar armadilhas também

		std::vector<LevelEnemySpawnerData> spawners;

		// --- Lógica para determinar o tipo de inimigo ---
		// Thresholds para introdução de inimigos mais fortes
		const float mediumThreshold = 0.33f;
		const float advancedThreshold = 0.66f;

		// Escolhe o tipo de inimigo Melee
		CEnemyType meleeToSpawn = meleeProgression.base;
		if ( overallProgress > advancedThreshold ) {
			// Chance de spawnar inimigo avançado aumenta conforme o progresso
			if ( getRandom( 0 , 100 ) < ( int ) ( ( ( overallProgress - advancedThreshold ) / ( 1.0f - advancedThreshold ) ) * 100.0f ) ) {
				meleeToSpawn = meleeProgression.advanced;
			}
			else {
				meleeToSpawn = meleeProgression.medium;
			}
		}
		else if ( overallProgress > mediumThreshold ) {
			// Chance de spawnar inimigo médio aumenta
			if ( getRandom( 0 , 100 ) < ( int ) ( ( ( overallProgress - mediumThreshold ) / ( advancedThreshold - mediumThreshold ) ) * 100.0f ) ) {
				meleeToSpawn = meleeProgression.medium;
			}
			else {
				meleeToSpawn = meleeProgression.base;
			}
		}

		// Escolhe o tipo de inimigo Ranged (lógica similar)
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

		// --- Lógica para determinar a quantidade de inimigos ---
		int currentSpawnCount = baseSpawnCount + ( int ) ( ( maxSpawnCount - baseSpawnCount ) * overallProgress );

		// Adiciona os spawners para a sala atual
		spawners.push_back( { meleeToSpawn, getRandom( currentSpawnCount / 2, currentSpawnCount ), 10, 5, 0, 0.0 } );
		spawners.push_back( { rangedToSpawn, getRandom( currentSpawnCount / 2, currentSpawnCount ), 10, 5, 0, 0.0 } );

		// Adiciona os spawners definidos aos dados do nível
		for ( const auto & spawner : spawners ) {
			if ( spawner.SpawnCount > 0 ) { // Só adiciona se houver inimigos para spawnar
				levelData.enemySpawners.emplace( spawner.type , spawner );
			}
		}

		// Cria o objeto da sala e o adiciona ao handler
		gameRoomLevel gameRoom( levelData , roomIndex + 1 );
		LevelHandler::Get( ).addLevel( type , gameRoom );
	}
}


void LevelManager::generateBossLevel( ) {
	// A geração do chefe pode permanecer mais estática, pois é uma sala única e especial.
	// Pode incluir inimigos avançados como minions.
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
	// Gera os 3 níveis com dificuldade progressiva
	const int totalProceduralLevels = 3;
	generateProceduralLevel( firstMap , "firstMap" , 0 , totalProceduralLevels , 2 , 5 );
	generateProceduralLevel( secondMap , "secondMap" , 1 , totalProceduralLevels , 3 , 7 );
	generateProceduralLevel( thirdMap , "thirdMap" , 2 , totalProceduralLevels , 5 , 10 );

	// Gera o nível do chefe separadamente
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
		restartLevels( ); // Reinicializa os níveis se já estiverem iniciados
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
		return; // Já está no último nível
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
