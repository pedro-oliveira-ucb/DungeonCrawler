#include "enemiesController.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../Managers/collisionManager/collisionManager.h"
#include "../../Managers/LevelManager/LevelManager.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/attackHandler/attackHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

void enemiesController::threadFunction( ) {

	Log::Print( "[enemiesController] Hello world!" );

	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );


	while ( true ) {

		if ( player == nullptr ) {
			player = entitiesHandler::Get( ).getLocalPlayer( );
			continue;
		}

		entitiesHandler::Get( ).updateSpawnedEnemies( player );

		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}

	Log::Print( "[enemiesController] bye!" );

}