#include "gameInitializer.h"

#include "../gameResources/gameResourceManager/gameResourceManager.h"
#include "../gameWindow/gameWindow.h"
#include "../gameWorld/gameWorld.h"
#include "../gameThreads/gameThreads.h"

#include "../World/World.h"

#include "../../../Utils/Log/Log.h"

#include "EntitiesInitializer/EntitiesInitializer.h"

gameResourceManager _gameResourceManager;
gameWindow _gameWindow;
gameWorld _gameWorld;
gameThreads _gameThreads;

EntitiesInitializer _entitiesInitializer;

bool gameInitializer::initializeResources( ) {
	world.resourceManagerPointer = &_gameResourceManager;
	world.gameWorldPointer = &_gameWorld;

	if ( !_gameResourceManager.initialize( "resources" ) ) {
		Log::Print( "[gameInitializer] gameResourceManager failed to initialize!" );
		return false;
	}

	Log::Print( "[gameInitializer] assets initialized!" );

	if ( !_entitiesInitializer.initialize(  ) ) {
		Log::Print( "[gameInitializer] EntitiesInitializer failed to initialize!" );
		return false;
	}

	Log::Print( "[gameInitializer] entities initialized!" );


	return true;
}

bool gameInitializer::initializeThreads( ) {
	return _gameThreads.deploy( );
}