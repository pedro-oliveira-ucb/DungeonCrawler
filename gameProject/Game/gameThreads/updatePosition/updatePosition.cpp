#include "updatePosition.h"
#include "../../../Globals/Globals.h"
#include "../../gameControls/keybindHandler/keybindHandler.h"





void updatePositionEvent::threadFunction( ) {
	while ( true ) {
	

		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	}
}