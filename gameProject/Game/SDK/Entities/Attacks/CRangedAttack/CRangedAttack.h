#pragma once
#include "../CBaseAttack/CBaseAttack.h"

class CRangedAttack : public CBaseAttack
{
	void otherActiveLogic( CBaseEntity * sender ) override;
	void otherDeactiveLogic( ) override;
public:
	CRangedAttack( CBaseEntityConstructor entityBuilder ,
		CBaseAttackConstructor attackBuilder );
	
	void updateAttackPosition( ) override; // Atualiza o estado do ataque
	std::shared_ptr<CBaseAttack> Clone( ) override;
};

