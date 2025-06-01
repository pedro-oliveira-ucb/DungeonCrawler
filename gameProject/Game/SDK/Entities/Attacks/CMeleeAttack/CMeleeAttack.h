#pragma once
#include "../CBaseAttack/CBaseAttack.h"

class CMeleeAttack : public CBaseAttack
{
	void otherActiveLogic( CBaseEntity * sender ) override;
	void otherDeactiveLogic( ) override;
	double lastUpdateTime = 0.0;
public:
	CMeleeAttack( CBaseEntityConstructor entityBuilder, 
		CBaseAttackConstructor attackBuilder );

	CMeleeAttack( const CMeleeAttack & ); // Construtor de cópia

	void updateEntity( ) override; // Atualiza a entidade

	std::shared_ptr<CBaseAttack> Clone( ) override;
};

