#pragma once
#include <chrono>
#include <vector>
#include <mutex> // Adicionado para uso de std::mutex
#include "../../SDK/math/Vector2D/GVector2D.h"
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"


struct LevelEnemySpawnerData {
	CEnemyType type;
	int SpawnCount;
	int RespawnCount;
	int RespawnTimer = 10;
	int respawnedCount = 0; // Contador de inimigos ressuscitados
	double lastRespawnTime;
};

struct LevelData {
	int levelNumber; // N�mero do n�vel
	std::string mapName; // Nome do mapa associado ao n�vel
	int Traps = 0; // Quantidade de armadilhas no n�vel
	std::unordered_map<CEnemyType , LevelEnemySpawnerData> enemySpawners; // Dados dos spawns de inimigos
};

class gameRoomLevel
{
	std::mutex m_mutex; // Mutex para proteger o acesso a dados compartilhados
	LevelData levelData; // Dados do n�vel atual
	int RoomID = 0; // ID do quarto atual
	bool initialized = false;
	std::unordered_map<CEnemyType , std::unordered_map<int, CEnemyEntity *>> enemies;

	void updateEntities( );
	void spawnEnemies( );
	void respawnEnemies( bool forceRespawn = false );
	GVector2D getValidPositionOnCurrentRoom( );
	void spawnTraps( );
	bool canRespawn( CEnemyType enemyType );
public:
	gameRoomLevel( LevelData levelData, int id) : levelData( levelData ), RoomID(id) { } // Construtor que recebe os dados do n�vel
	gameRoomLevel( const gameRoomLevel & other );

	LevelData & getLevelData( ) { return levelData; } // M�todo para acessar os dados do n�vel
	
	void init( );
	void restart( );
	void update( );
	bool hasEnemyAlive( );
	int getRoomID( ) const { return RoomID; } // M�todo para obter o ID do quarto atual
};

