#include "gameInitializer.h"

#include "../Managers/gameResourceManager/gameResourceManager.h"
#include "../gameWindow/gameWindow.h"
#include "../gameThreads/gameThreads.h"

#include "../../../Utils/Log/Log.h"

#include "EntitiesInitializer/EntitiesInitializer.h"

gameResourceManager _gameResourceManager;
gameWindow _gameWindow;
gameThreads _gameThreads;

EntitiesInitializer _entitiesInitializer;

bool gameInitializer::initializeResources( ) {

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