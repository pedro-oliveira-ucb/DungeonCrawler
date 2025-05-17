#include "Game.h"
#include "gameInitializer/gameInitializer.h"
#include "../Utils/Log/Log.h"

bool Game::isResourcesLoaded( ) {
	return this->resourcesLoaded;
}

bool Game::start( ) {
	if ( !gameInitializer::Get( ).initializeResources( ) ) {
		Log::Print( "[Game] resources initialization failed!" );
		return false;
	}
	this->resourcesLoaded = true;
	

	return true;
}