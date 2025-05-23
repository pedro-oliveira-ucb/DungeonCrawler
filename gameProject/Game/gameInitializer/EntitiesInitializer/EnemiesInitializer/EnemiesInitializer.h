#pragma once
#include <string>
#include "../../../Utils/singleton.h"

class CEnemyEntity;

class EnemiesInitializer : public CSingleton<EnemiesInitializer>
{
private:
    CEnemyEntity* generateEnemy(const std::string& animationName);
    bool initializeEvents(std::string enemyName);

public:
    bool initialize();
};

