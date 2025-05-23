#pragma once
#include <vector>
#include <mutex> // Adicionado para uso de std::mutex
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"

struct LevelData {
    int levelNumber;
    std::string mapName;
    int enemyCount;
    int RespawnCount;
    // Outras propriedades como spawn points, boss, etc.
};

class LevelManager {
private:
    // Variáveis que precisam ser protegidas para evitar condições de corrida
    std::vector<LevelData> levels;
    int currentLevelIndex = 0;
    std::vector<std::shared_ptr<CEnemyEntity>> enemies;

    bool started = false;
    int respawnCount = 0;

    // Mutex para proteger as variáveis acima
    mutable std::mutex managerMutex;

    // Métodos de acesso podem agora usar lock para thread safety
    const LevelData & getCurrentLevel( ) const;
    void moveToNextLevel( );
    bool isLastLevel( ) const;
    bool hasEnemyAlive( );

    void updateEnemies( );

    void spawnEnemiesForLevel( const LevelData & data );

public:
    LevelManager();
    void loadLevels();
    void updateLevel( );
	std::vector<std::shared_ptr< CEnemyEntity>> getEnemies( ) const;
};

extern LevelManager levelManager;