#include "CBaseItem.h"

#include "../../../Events/EventManager.h"

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

void CBaseItem::Active( GVector2D position) {
	std::lock_guard<std::mutex> baseItemLock( baseItemMutex );
	this->setEntityPosition( position );
	this->active = true;
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_active" );
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

