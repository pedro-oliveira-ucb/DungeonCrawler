#include "trapsHandler.h"


#include "../../../Utils/utils.h"
#include "../../../Utils/Log/Log.h"

#include "../entitiesHandler/entitiesHandler.h"

#include "../../Managers/collisionManager/collisionManager.h"

void trapsHandler::updateTraps( ) {

	std::lock_guard<std::mutex> lock( handlerMutex );
	CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );
	if ( localPlayer == nullptr )
		return;

	if ( this->spawnedTraps.empty( ) ) {
		return;
	}

	auto it = spawnedTraps.begin( );
	while ( it != spawnedTraps.end( ) ) {
		CBaseTrap * trap = it->get( );
		if ( !trap || !trap->getEntityAnimations( ) ) {
			it = spawnedTraps.erase( it );
			continue;
		}

		if ( trap->isActive( ) ) {
			trap->updateActivatedTrap ( );
		}
		else if ( CollisionManager::Get( ).checkCollision( trap , localPlayer , trap->getEntityPosition( ) ) ) {
			trap->activeTrap( );
		}

		++it;
	}

}

std::vector<std::unique_ptr<CBaseTrap>> * trapsHandler::getSpawnedTraps( )
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	return &spawnedTraps;
}

void trapsHandler::addSpawnableTrap( std::unique_ptr<CBaseTrap> trap )
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	spawnableTraps.emplace( trap->getTrapType( ) , std::move( trap ) );
}

void trapsHandler::spawnTrap( TrapType itemType ) {
	std::lock_guard<std::mutex> lock( handlerMutex );

	auto it = spawnableTraps.find( itemType );
	if ( it == spawnableTraps.end( ) )
		return;

	if ( it->second.get( ) != nullptr ) {
		std::unique_ptr<CBaseTrap> trapClone = it->second->TrapClone( );
		Log::Print( "Copy complete, trying to active item" );
		trapClone->initialize( );
		Log::Print( "Item activated, trying to push to vector" );
		spawnedTraps.push_back( std::move( trapClone ) );
	}
}

std::unique_ptr<CBaseTrap> * trapsHandler::getSpawnableTrap( TrapType traptype )
{
	std::lock_guard<std::mutex> lock( handlerMutex );
	auto it = spawnableTraps.find( traptype );
	if ( it != spawnableTraps.end( ) ) {
		return &it->second;
	}
	return nullptr;
}