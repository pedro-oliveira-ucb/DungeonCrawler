#include "CBaseItem.h"

#include "../../../Events/EventManager.h"
#include "../../../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../../../../../Utils/Log/Log.h"

CBaseItem::CBaseItem( CBaseEntityConstructor superConstructor, ItemType itemType )
	: CBaseEntity( superConstructor ), type(itemType ), active(false )
{
	
}

CBaseItem::CBaseItem( const CBaseItem & other )
	: CBaseEntity( other ) ,
	type( other.type ) ,
	active( false )
{

}

void CBaseItem::applyEffect( ) {
	

}

void CBaseItem::Active() {
	std::lock_guard<std::mutex> baseItemLock( baseItemMutex );
	Log::Print( "Activating item, getting random place" );
	GVector2D position = entitiesHandler::Get().getRandomPlaceAroundPlayer( 500 );
	Log::Print( "Got random place, setting entity position" );
	this->setEntityPosition( position );
	Log::Print( "Entity position setted, activating item" );
	this->active = true;
	Log::Print( "Item activated, triggering active event" );
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_active" );
	Log::Print( "Trigged event, item actived!" );
}

bool CBaseItem::isActive( ) {
	std::lock_guard<std::mutex> baseItemLock(baseItemMutex );
	return this->active;
}

void CBaseItem::Deactive( ) {
	std::lock_guard<std::mutex> baseItemLock( baseItemMutex );
	this->active = false;
}

ItemType CBaseItem::getItemType( ) {
	std::lock_guard<std::mutex> baseItemLock( baseItemMutex );
	return this->type;
}

