#pragma once
#include "../CBaseTrap/CBaseTrap.h"

class CSpikeTrap : public CBaseTrap
{
	int damageAmount;
	bool onReverse = false;
	bool appliedEffect = false;
public:
	CSpikeTrap( CBaseEntityConstructor , TrapType itemType , int damageAmount );
	CSpikeTrap( const CSpikeTrap & other );

	void updateActivatedTrap( ) override;

	void applyEffect( ) override;
	std::unique_ptr<CBaseTrap> TrapClone( ) const override;
};



