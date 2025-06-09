#pragma once
#include <chrono>
#include <vector>
#include <mutex> // Adicionado para uso de std::mutex
#include <unordered_map>
#include "../../gameObjects/gameRoomLevel/gameRoomLevel.h"
#include "../../gameObjects/mapObject/rMapObject.h"

class LevelManager {
private:
    std::mutex m_mutex;
	int currentLevel = 0;
	std::unordered_map< mapType , std::unordered_map<int ,gameRoomLevel>> mapLevels;

	void generateBossLevel( );

	void generateLevels( );
	void initializeLevels( );

	bool started = false;
	void generateProceduralLevel( mapType type , std::string name , int levelIndex , int totalLevels , int baseSpawnCount , int maxSpawnCount );
public:
    void initialize( ); // inicializa cada nivel
	void update( ); // atualiza cada nivel
	void start( );
	void moveToNextLevel( ); // move para o proximo nivel
	bool hasAliveEnemyOnCurrentRoom(int roomID );
	int roomsInCurrentLevel( );
};

extern LevelManager levelManager;