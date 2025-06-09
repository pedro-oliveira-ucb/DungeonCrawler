#pragma once
#include <string>
#include "../../../../Utils/singleton.h"

class CEnemyEntity;

class EnemiesInitializer : public CSingleton<EnemiesInitializer>
{
private:
   

public:
    static bool initializeEvents( std::string enemyName );
    bool initialize();
};

