#pragma once
#include "../../../../Utils/singleton.h"
#include "../../../SDK/Entities/Attacks/CMeleeAttack/CMeleeAttack.h"
#include "../../../SDK/Entities/Attacks/CRangedAttack/CRangedAttack.h"

struct CBaseAttackConstructor;

class AttacksInitializer : public CSingleton<AttacksInitializer>
{
	template <typename T>
	T * generateAttack( std::string animationName , CBaseAttackConstructor attackBuilder );
	template <typename T>
	bool generateMobAttack( CBaseAttackConstructor , std::string mobName );
	template <typename T>
	bool generateLocalPlayerAttack( CBaseAttackConstructor );

	bool generateEnemiesAttacks( );
	bool generateLocalPlayerAttacks( );

public:
	bool initializeAttacks( );
};
