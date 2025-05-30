#pragma once
#include <string>
#include "../../../../Utils/singleton.h"

class CEnemyEntity;

class EnemiesInitializer : public CSingleton<EnemiesInitializer>
{
private:
    bool initializeEvents(std::string enemyName);

public:
    bool initialize();
};

