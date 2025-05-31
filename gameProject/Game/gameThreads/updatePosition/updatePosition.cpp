#include "updatePosition.h"

#include <cmath>

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"



void updatePositionEvent::threadFunction( ) {
	Log::Print( "[updatePositionEvent] Hello world!" );
	keybindHandler::Get( ).initializeKeybinds( );

	while ( true ) {
		if ( globals.gamePaused )
			continue;

		entitiesHandler::Get( ).updateLocalPlayer( );

		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}

	Log::Print( "[updatePositionEvent] bye!" );
}