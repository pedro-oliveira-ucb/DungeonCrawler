#include "CBaseTrap.h"


#include "../../../Events/EventManager.h"
#include "../../../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../../../../Utils/Log/Log.h"

CBaseTrap::CBaseTrap( CBaseEntityConstructor superConstructor , TrapType trapType )
	: CBaseEntity( superConstructor ) , type( trapType ) , initialized( false )
{

}

CBaseTrap::CBaseTrap( const CBaseTrap & other )
	: CBaseEntity( other ) ,
	type( other.type ) ,
	initialized( false )
{

}

void CBaseTrap::applyEffect( ) {


}

void CBaseTrap::initialize( ) {
	std::lock_guard<std::mutex> baseItemLock( baseTrapMutex );
	Log::Print( "Activating item, getting random place" );
	GVector2D position = entitiesHandler::Get( ).getRandomPlaceAroundPlayer( 500 );
	Log::Print( "Got random place, setting entity position" );
	this->setEntityPosition( position );
	Log::Print( "Entity position setted, activating item" );
	this->initialized = true;
	this->getEntityAnimations( )->setAnimationStep( 1 );
	Log::Print( "Item activated, triggering active event" );
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_active" );
	Log::Print( "Trigged event, item actived!" );
}

bool CBaseTrap::isInitialized( ) const {
	std::lock_guard<std::mutex> baseItemLock( baseTrapMutex );
	return this->initialized;
}

void CBaseTrap::unitialize( ) {
	std::lock_guard<std::mutex> baseItemLock( baseTrapMutex );
	this->initialized = false;
}

void CBaseTrap::activeTrap( )
{
	std::lock_guard<std::mutex> baseItemLock( baseTrapMutex );
	this->activated = true;
}

void CBaseTrap::deactivateTrap( )
{
	std::lock_guard<std::mutex> baseItemLock( baseTrapMutex );
	this->activated = false;
}

bool CBaseTrap::isActive( ) const
{
	std::lock_guard<std::mutex> baseItemLock( baseTrapMutex );
	return this->activated;
}

TrapType CBaseTrap::getTrapType( ) const {
	std::lock_guard<std::mutex> baseItemLock( baseTrapMutex );
	return this->type;
}

