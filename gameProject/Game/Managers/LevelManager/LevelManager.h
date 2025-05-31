#pragma once
#include <chrono>
#include <vector>
#include <mutex> // Adicionado para uso de std::mutex
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"

struct LevelData {
	int levelNumber; // Número do nível
	std::string mapName; // Nome do mapa associado ao nível
	int enemyCount; // Número de inimigos no nível
	int RespawnCount; // Número de ressucitações permitidas
	int RespawnTimer = 10; // Tempo de respawn em segundos
	int Traps = 0; // Quantidade de armadilhas no nível
    // Outras propriedades como spawn points, boss, etc.
};

class LevelManager {
private:
    // Variáveis que precisam ser protegidas para evitar condições de corrida
    std::vector<LevelData> levels;
    int currentLevelIndex = 0;
    std::vector<CEnemyEntity* > enemies;

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

    void spawnEnemiesForLevel( const LevelData & data );

public:
    LevelManager();
    void loadLevels();
    void updateLevel( );
};

extern LevelManager levelManager;