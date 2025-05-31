#pragma once
#include <chrono>
#include <vector>
#include <mutex> // Adicionado para uso de std::mutex
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"


struct LevelEnemySpawnerData {
    CEnemyType type;
    int SpawnCount;
	int RespawnCount;
    int RespawnTimer = 10;
	int respawnedCount = 0; // Contador de inimigos ressuscitados
    std::chrono::steady_clock::time_point timeSinceLastRespawn;
};

struct LevelData {
	int levelNumber; // Número do nível
	std::string mapName; // Nome do mapa associado ao nível
	int Traps = 0; // Quantidade de armadilhas no nível
	std::vector<LevelEnemySpawnerData> enemySpawners; // Dados dos spawns de inimigos
};

class LevelManager {
private:
    // Variáveis que precisam ser protegidas para evitar condições de corrida
    std::vector<LevelData> levels;
    int currentLevelIndex = 0;
    std::unordered_map<CEnemyType, std::vector<CEnemyEntity*> > enemies;

    bool started = false;
    int respawnCount = 0;
    std::chrono::steady_clock::time_point respawnTimer;

    // Mutex para proteger as variáveis acima
    std::mutex managerMutex;

    // Métodos de acesso podem agora usar lock para thread safety
     LevelData getCurrentLevel( ) ;
    void moveToNextLevel( );
    bool isLastLevel( );
    bool hasEnemyAlive( );

    void updateEnemies( );

    bool canRespawn( CEnemyType enemyType );

    void spawnEnemiesForLevel( const LevelData & data );
    void generateLevel1( );
    void generateLevel2( );

public:
    LevelManager();
    void loadLevels();
    void updateLevel( );
};

extern LevelManager levelManager;