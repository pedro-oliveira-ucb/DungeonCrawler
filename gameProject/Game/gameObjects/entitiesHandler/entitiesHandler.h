#pragma once
#include "../../../Utils/singleton.h"
#include "../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"
#include "../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"
#include <vector>

class entitiesHandler : public CSingleton<entitiesHandler> {
private:
    CPlayerEntity* localPlayer = nullptr;
    std::vector<CEnemyEntity*> enemies;

public:
    void setLocalPlayer(CPlayerEntity* player) {
        localPlayer = player;
    }
    CPlayerEntity* getLocalPlayer() {
        return localPlayer;
    }

    void addEnemy(CEnemyEntity* enemy) {
        enemies.push_back(enemy);
    }
    std::vector<CEnemyEntity*>& getEnemies() {
        return enemies;
    }
};

