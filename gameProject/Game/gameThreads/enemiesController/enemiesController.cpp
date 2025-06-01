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
		currentGameStateType gameState = Globals::Get( ).getCurrentGameState( )->getCurrentGameState( );

		if ( gameState == currentGameStateType::GAME_STATE_PAUSED || gameState == currentGameStateType::GAME_STATE_NONE ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
			continue;
		}

		if ( player == nullptr ) {
			player = entitiesHandler::Get( ).getLocalPlayer( );
			continue;
		}


		entitiesHandler::Get( ).updateSpawnedEnemies( player );

		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}

	Log::Print( "[enemiesController] bye!" );

}