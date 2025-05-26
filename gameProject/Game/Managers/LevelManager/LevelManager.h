#pragma once
#include <chrono>
#include <vector>
#include <mutex> // Adicionado para uso de std::mutex
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"

struct LevelData {
    int levelNumber;
    std::string mapName;
    int enemyCount;
    int RespawnCount;
    int RespawnTimer = 10;
    // Outras propriedades como spawn points, boss, etc.
};

class LevelManager {
private:
    // Vari�veis que precisam ser protegidas para evitar condi��es de corrida
    std::vector<LevelData> levels;
    int currentLevelIndex = 0;
    std::vector<std::shared_ptr<CEnemyEntity>> enemies;

    bool started = false;
    int respawnCount = 0;
    std::chrono::steady_clock::time_point respawnTimer;

    // Mutex para proteger as vari�veis acima
    std::mutex managerMutex;

    // M�todos de acesso podem agora usar lock para thread safety
     LevelData getCurrentLevel( ) ;
    void moveToNextLevel( );
    bool isLastLevel( );
    bool hasEnemyAlive( );

    void updateEnemies( );

    void spawnEnemiesForLevel( const LevelData & data );

public:
    LevelManager();
    void loadLevels();
    void updateLevel( );
	std::vector<std::shared_ptr< CEnemyEntity>> getEnemies( );
};

extern LevelManager levelManager;