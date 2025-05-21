#pragma once
#include "../../../Utils/singleton.h"
#include "../../../SDK/Entities/Attacks/CMeleeAttack/CMeleeAttack.h"
#include "../../../SDK/Entities/Attacks/CRangedAttack/CRangedAttack.h"

class AttacksInitializer : public CSingleton<AttacksInitializer>
{
	template <typename T>
	T * generateAttack( std::string animationName , CBaseAttackConstructor attackBuilder );



	bool generateLocalPlayerAttacks( );

public:
	bool initializeAttacks( );
};
