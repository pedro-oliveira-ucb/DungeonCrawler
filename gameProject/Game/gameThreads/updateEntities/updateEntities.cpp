#include "updateEntities.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"
#include "../../gameWorld/gameWorld.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"
#include "../../gameWorld/gameWorld.h"

void updateEntities::threadFunction( ) {

	Log::Print( "[updateEntities] Hello world!" );

	while ( true ) {

		//if(_gameWorld.localplayer. )


		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}