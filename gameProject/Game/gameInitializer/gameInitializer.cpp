#include "gameInitializer.h"

#include "../gameResources/gameResourceManager/gameResourceManager.h"
#include "../gameWindow/gameWindow.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

gameResourceManager _gameResourceManager;
gameWindow _gameWindow;

bool gameInitializer::initializeResources( ) {

	if ( !_gameResourceManager.initialize( "resources" ) ) {
		Log::Print( "[gameInitializer] gameResourceManager failed to initialize!" );
		return false;
	}

	globals.resourceManagerPointer = &_gameResourceManager;

	return true;
}