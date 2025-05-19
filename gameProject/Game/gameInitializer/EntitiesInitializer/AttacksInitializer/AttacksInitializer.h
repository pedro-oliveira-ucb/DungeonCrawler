#pragma once
#include "../../../Utils/singleton.h"
#include "../../../SDK/Entities/Attacks/CBaseAttack/CBaseAttack.h"

class AttacksInitializer : public CSingleton<AttacksInitializer>
{
public:
	CBaseAttack * generate( std::string animationName, CBaseAttackConstructor attackBuilder );
};
