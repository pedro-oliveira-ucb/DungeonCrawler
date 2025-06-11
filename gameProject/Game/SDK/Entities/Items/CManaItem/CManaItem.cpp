#include "CManaItem.h"

#include <algorithm>

#include "../../../../../Globals/Globals.h"

#include "../../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../Events/EventManager.h"


CManaItem::CManaItem( CBaseEntityConstructor superConstructor , ItemType itemType )
	: CBaseItem( superConstructor , itemType )
{
}

CManaItem::CManaItem( const CManaItem & other )
	: CBaseItem( other )
{

}

std::unique_ptr<CBaseItem> CManaItem::ItemClone( ) const
{
	std::unique_ptr<CManaItem> clone = std::make_unique<CManaItem>( *this ); // Usa cópia
	return clone;
}

void CManaItem::applyEffect( ) {
	Globals::Get( ).getGame( )->addMana( 5 );

	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_effect" );
}
