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
	GVector2D position = entitiesHandler::Get( ).getRandomPlaceAroundPlayer( 500 );
	this->setEntityPosition( position );
	this->initialized = true;
	this->getEntityAnimations( )->setAnimationStep( 1 );
	EventManager::Get( ).Trigger( this->GetEntityName( ) + "_active" );
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

