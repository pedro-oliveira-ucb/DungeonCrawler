#pragma once

#include "../../../Utils/singleton.h"
#include "../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <memory>

class entitiesHandler : public CSingleton<entitiesHandler> {
private:
    CPlayerEntity * localPlayer = nullptr;
    std::unordered_map<CEnemyType , std::unique_ptr<CEnemyEntity>> enemies;
    std::vector<std::unique_ptr<CBaseEntity>> spawnedEntities;
    std::vector<std::unique_ptr<CEnemyEntity>> spawnedEnemies;
    mutable std::mutex handlerMutex;

public:
    void setLocalPlayer( CPlayerEntity * player );
    CPlayerEntity * getLocalPlayer( );

    void addEnemy( CEnemyType type , std::unique_ptr<CEnemyEntity> enemy );
    std::unordered_map<CEnemyType , std::unique_ptr<CEnemyEntity>> * getEnemies( );

    void addSpawnedEnemy( std::unique_ptr<CEnemyEntity> * enemy );
    std::vector<std::unique_ptr<CEnemyEntity>> * getSpawnedEnemies( );

    void addSpawnedEntity( std::unique_ptr<CBaseEntity> entity );
    std::vector<std::unique_ptr<CBaseEntity>> * getSpawnedEntities( );

    void updateSpawnedEnemies( CPlayerEntity* localPlayer  );
    void updateEnemiesCollision( );
	void updateLocalPlayer( );
};
