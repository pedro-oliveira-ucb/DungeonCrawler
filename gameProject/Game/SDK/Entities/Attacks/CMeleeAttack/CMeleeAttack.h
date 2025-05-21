#pragma once
#include "../CBaseAttack/CBaseAttack.h"

class CMeleeAttack : public CBaseAttack
{
	void otherActiveLogic( CBaseEntity * sender ) override;
	void otherDeactiveLogic( ) override;
public:
	CMeleeAttack( CBaseEntityConstructor entityBuilder, 
		CBaseAttackConstructor attackBuilder );

	CMeleeAttack( const CMeleeAttack & ); // Construtor de cópia


	void updateAttackPosition( ) override; // Atualiza o estado do ataque
	std::shared_ptr<CBaseAttack> Clone( ) override;
};

