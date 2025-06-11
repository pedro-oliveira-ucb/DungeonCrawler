#include "CKeyItem.h"

#include <algorithm>

#include "../../../../../Globals/Globals.h"

#include "../../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Events/EventManager.h"


CKeyItem::CKeyItem( CBaseEntityConstructor superConstructor , ItemType itemType )
	: CBaseItem( superConstructor , itemType ) 
{

}

CKeyItem::CKeyItem( const CKeyItem & other )
	: CBaseItem( other )
{
	
}

std::unique_ptr<CBaseItem> CKeyItem::ItemClone( ) const{
	std::unique_ptr<CKeyItem> clone = std::make_unique<CKeyItem>( *this ); // Usa cópia
	return clone;
}

void CKeyItem::applyEffect( ) {
	// Add the key to the game state
	Globals::Get( ).getGame( )->addKey( 2 );
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_effect" );
}
