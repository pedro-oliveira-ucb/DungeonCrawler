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
	int levelNumber; // Número do nível
	std::string mapName; // Nome do mapa associado ao nível
	int Traps = 0; // Quantidade de armadilhas no nível
	std::unordered_map<CEnemyType , LevelEnemySpawnerData> enemySpawners; // Dados dos spawns de inimigos
};

class gameRoomLevel
{
	std::mutex m_mutex; // Mutex para proteger o acesso a dados compartilhados
	LevelData levelData; // Dados do nível atual
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
	gameRoomLevel( LevelData levelData, int id) : levelData( levelData ), RoomID(id) { } // Construtor que recebe os dados do nível
	gameRoomLevel( const gameRoomLevel & other );

	LevelData & getLevelData( ) { return levelData; } // Método para acessar os dados do nível
	
	void init( );
	void restart( );
	void update( );
	bool hasEnemyAlive( );
	int getRoomID( ) const { return RoomID; } // Método para obter o ID do quarto atual
};

