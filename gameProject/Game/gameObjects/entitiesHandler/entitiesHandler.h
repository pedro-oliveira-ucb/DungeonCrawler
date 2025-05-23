#pragma once
#include "../../../Utils/singleton.h"
#include "../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"
#include <vector>
#include <mutex>

class entitiesHandler : public CSingleton<entitiesHandler> {
private:
    CPlayerEntity * localPlayer = nullptr;
    std::vector<CEnemyEntity *> enemies;
    std::vector<CBaseEntity *> spawnedEntities;
    mutable std::mutex handlerMutex;

public:
    void setLocalPlayer( CPlayerEntity * player ) {
        std::lock_guard<std::mutex> lock( handlerMutex );
        localPlayer = player;
    }
    CPlayerEntity * getLocalPlayer( ) {
        std::lock_guard<std::mutex> lock( handlerMutex );
        return localPlayer;
    }

    void addEnemy( CEnemyEntity * enemy ) {
        std::lock_guard<std::mutex> lock( handlerMutex );
        enemies.push_back( enemy );
    }
    std::vector<CEnemyEntity *> getEnemies( ) {
        std::lock_guard<std::mutex> lock( handlerMutex );
        return enemies;
    }

    void addSpawnedEntity( CBaseEntity * entity ) {
        std::lock_guard<std::mutex> lock( handlerMutex );
        spawnedEntities.push_back( entity );
    }
    std::vector<CBaseEntity *> getSpawnedEntities( ) {
        std::lock_guard<std::mutex> lock( handlerMutex );

        // Remove ponteiros nulos do vetor
        spawnedEntities.erase(
            std::remove_if(
                spawnedEntities.begin( ) ,
                spawnedEntities.end( ) ,
                [ ] ( const CBaseEntity * enemy ) { return enemy == nullptr; }
            ) ,
            spawnedEntities.end( )
        );

        return spawnedEntities;
    }
};