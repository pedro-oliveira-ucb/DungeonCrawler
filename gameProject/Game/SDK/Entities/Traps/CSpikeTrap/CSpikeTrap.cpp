#include "CSpikeTrap.h"

#include <algorithm>

#include "../../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Events/EventManager.h"


CSpikeTrap::CSpikeTrap( CBaseEntityConstructor superConstructor , TrapType itemType , int dmg )
	: CBaseTrap( superConstructor , itemType ) , damageAmount( dmg )
{
}

CSpikeTrap::CSpikeTrap( const CSpikeTrap & other )
	: CBaseTrap( other )
{
	this->damageAmount = other.damageAmount;
}

std::unique_ptr<CBaseTrap> CSpikeTrap::TrapClone( ) const
{
	std::unique_ptr<CSpikeTrap> clone = std::make_unique<CSpikeTrap>( *this ); // Usa cópia
	return clone;
}

void CSpikeTrap::updateActivatedTrap( ) {
	if ( !this->isActive( ) ) {
		return;
	}
	// If the trap is active, we can apply the effect
	if ( !appliedEffect ) {
		applyEffect( );
		appliedEffect = true;
	}

	if ( this->getEntityAnimations( )->isAnimationFinished( ) ) {
		if ( !this->onReverse )
			this->onReverse = true;
		else {
			this->deactivateTrap( );
			// appliedEffect = false;
			this->onReverse = false;
		}
	}
	else
		this->getEntityAnimations( )->updateAnimation( false , this->onReverse );
}

void CSpikeTrap::applyEffect( ) {
	CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );
	if ( localPlayer == nullptr ) {
		return;
	}

	localPlayer->Hit( damageAmount );
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_effect" );
}
