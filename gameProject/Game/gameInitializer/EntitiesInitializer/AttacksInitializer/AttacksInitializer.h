#pragma once
#include "../../../../Utils/singleton.h"
#include "../../../SDK/Entities/Attacks/CMeleeAttack/CMeleeAttack.h"
#include "../../../SDK/Entities/Attacks/CRangedAttack/CRangedAttack.h"

struct CBaseAttackConstructor;

class AttacksInitializer : public CSingleton<AttacksInitializer>
{


	bool generateEnemiesAttacks( );
	bool generateLocalPlayerAttacks( );

public:
	template <typename T>
	static T * generateAttack( std::string animationName , CBaseAttackConstructor attackBuilder );
	template <typename T>
	static bool generateMobAttack( CBaseAttackConstructor , std::string mobName );
	template <typename T>
	static bool generateLocalPlayerAttack( CBaseAttackConstructor );

	bool initializeAttacks( );
};
