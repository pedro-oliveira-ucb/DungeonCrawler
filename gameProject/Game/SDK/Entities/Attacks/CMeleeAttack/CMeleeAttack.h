#pragma once
#include "../CBaseAttack/CBaseAttack.h"

class CMeleeAttack : public CBaseAttack
{
	void otherActiveLogic( CBaseEntity * sender ) override;
	void otherDeactiveLogic( ) override;
	std::chrono::steady_clock::time_point lastUpdateTime;
public:
	CMeleeAttack( CBaseEntityConstructor entityBuilder, 
		CBaseAttackConstructor attackBuilder );

	CMeleeAttack( const CMeleeAttack & ); // Construtor de cópia

	void updateEntity( ) override; // Atualiza a entidade

	std::shared_ptr<CBaseAttack> Clone( ) override;
};

