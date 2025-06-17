#include "gameInitializer.h"

#include "../Managers/gameResourceManager/gameResourceManager.h"
#include "../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"

#include "../SDK/Events/EventManager.h"
#include "../gameThreads/gameThreads.h"

#include "../../../../Utils/Log/Log.h"

#include "EntitiesInitializer/EntitiesInitializer.h"
#include "ShadersInitializer/shadersInitializer.h"
#include "mapInitializer/mapInitializer.h"


gameResourceManager _gameResourceManager;
gameThreads _gameThreads;

EntitiesInitializer _entitiesInitializer;
shadersInitializer _shadersInitializer;
mapInitializer _mapInitializer;

bool gameInitializer::initializeEvents( ) {

	std::vector<std::string> eventsNames {
		"dialog_Update",
		"menu_gameStart"
	};

	for ( std::string event : eventsNames ) {
		std::string eventName = event;
		EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
			eventName ,
			[ eventName ] ( ) {
				gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
			}
		) );
	}

	Log::Print( "[gameInitializer] Events initialized!" );
	return true;
}

bool gameInitializer::initializeResources( ) {
	if ( !_gameResourceManager.initialize( "resources" ) ) {
		Log::Print( "[gameInitializer] gameResourceManager failed to initialize!" );
		return false;
	}
	Log::Print( "[gameInitializer] assets initialized!" );


	if ( !_shadersInitializer.initialize( ) ) {
		Log::Print( "[gameInitializer] shadersInitializer failed to initialize!" );
		return false;
	}
	Log::Print( "[gameInitializer] shaders initialized!" );


	if ( !_entitiesInitializer.initialize(  ) ) {
		Log::Print( "[gameInitializer] EntitiesInitializer failed to initialize!" );
		return false;
	}
	Log::Print( "[gameInitializer] entities initialized!" );


	if(!_mapInitializer.initialize( )) {
		Log::Print( "[gameInitializer] mapInitializer failed to initialize!" );
		return false;
	}
	Log::Print( "[gameInitializer] map objects initialized!" );


	if ( !initializeEvents( ) ) {
		Log::Print( "[gameInitializer] EventsInitializer failed to initialize!" );
		return false;
	}
	Log::Print( "[gameInitializer] entities initialized!" );


	if ( !initializeEvents( ) ) {
		Log::Print( "[gameInitializer] EventsInitializer failed to initialize!" );
		return false;
	}
	Log::Print( "[gameInitializer] entities initialized!" );

	return true;
}

bool gameInitializer::initializeThreads( ) {
	return _gameThreads.deploy( );
}