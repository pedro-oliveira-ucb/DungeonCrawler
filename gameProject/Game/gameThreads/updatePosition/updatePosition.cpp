#include "updatePosition.h"
#include "../../../Globals/Globals.h"
#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../../Utils/Log/Log.h"




void updatePositionEvent::threadFunction( ) {

	Log::Print( "[updatePositionEvent] Hello world!" );
	keybindHandler::Get( ).initializeKeybinds( );
	
	while ( true ) {
	
		if ( keybindHandler::Get( ).isPressed( UPDATE_MENU ) ) {
			globals.updateWindow = true;
		}


		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}