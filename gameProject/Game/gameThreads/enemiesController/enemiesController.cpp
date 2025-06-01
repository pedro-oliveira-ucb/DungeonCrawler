#include "enemiesController.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"

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
		currentGameState gameState = Globals::Get( ).getGame( )->getCurrentGameState( );

		if ( gameState != currentGameState::GAME_STATE_PLAYING ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
			continue;
		}

		if ( player == nullptr ) {
			player = entitiesHandler::Get( ).getLocalPlayer( );
			continue;
		}


		entitiesHandler::Get( ).updateSpawnedEnemies( player );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	Log::Print( "[enemiesController] bye!" );

}