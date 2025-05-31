#include "CHealthItem.h"

#include <algorithm>

#include "../../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Events/EventManager.h"


CHealthItem::CHealthItem( CBaseEntityConstructor superConstructor , ItemType itemType, int heal )
	: CBaseItem( superConstructor, itemType) , healAmount( heal )
{
}

CHealthItem::CHealthItem(const CHealthItem & other )
	: CBaseItem( other ) 
{
	this->healAmount = other.healAmount;
}

std::unique_ptr<CBaseItem> CHealthItem::ItemClone( ) const
{
	std::unique_ptr<CHealthItem> clone = std::make_unique<CHealthItem>( *this ); // Usa cópia
	return clone;
}

void CHealthItem::applyEffect( ) {
	CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );
	if ( localPlayer == nullptr ) {
		return;
	}

	int localHealth = localPlayer->getHealth( );
	int maxHealth = localPlayer->getMaxHealth( );
	int desiredHealth = std::clamp( localHealth + healAmount , 0 , maxHealth ); 
	localPlayer->setHealth( desiredHealth );
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_effect" );
}
