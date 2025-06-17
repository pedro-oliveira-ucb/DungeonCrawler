#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "../../../Utils/singleton.h"

#include "../../SDK/Entities/Attacks/CBaseAttack/CBaseAttack.h"

struct availableAttackHolder {
    CBaseAttackType type;
    std::shared_ptr<CBaseAttack> attack;
    availableAttackHolder( CBaseAttackType t , std::shared_ptr<CBaseAttack> a )
        : type( t ) , attack( a ) {
    }
};

class attackHandler : public CSingleton<attackHandler>
{
    std::vector<std::pair<CBaseEntity * , std::shared_ptr<CBaseAttack>>> runningAttacks;

    std::unordered_map<std::string, std::vector<availableAttackHolder>> availableEnemiesAttacks;
    std::unordered_map<CBaseAttackType, std::shared_ptr<CBaseAttack>> availableLocalPlayerAttacks;

public:
    int runningAttacksSize( );
    std::shared_ptr<CBaseAttack> getRunningAttack( int index );

    // Retorna todos os ataques que o Player pode usar
    std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> * getAvailableLocalPlayerAttack( );

    // Retorna um ataque que o inimigo pode usar (aqui só 1 por exemplo)
    std::unordered_map<CBaseAttackType , std::shared_ptr<CBaseAttack>> getAvailableEnemyAttack( std::string enemyName );

    void addAvailableLocalPlayerAttack( std::shared_ptr<CBaseAttack> attack );
    void addAvailableEnemyAttack( std::string enemyName, std::shared_ptr<CBaseAttack> attack );
    void clearAttacks( );
    void updateAttacks( );
    void throwNewAttack( CBaseEntity * sender , CBaseAttack * attack );
};
