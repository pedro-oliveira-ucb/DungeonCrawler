#include "updateEntities.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"

#include "../../Managers/collisionManager/collisionManager.h"
#include "../../Managers/LevelManager/LevelManager.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/attackHandler/attackHandler.h"
#include "../../Handlers/itemsHandler/itemsHandler.h"
#include "../../Handlers/trapsHandler/trapsHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

void updateEntities::threadFunction( ) {

	Log::Print( "[updateEntities] Hello world!" );

	while ( true ) {
		currentGameState gameState = Globals::Get( ).getGame( )->getCurrentGameState( );

		if ( gameState != currentGameState::GAME_STATE_PLAYING  ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
			continue;
		} 

		attackHandler::Get( ).updateAttacks( );
		itemsHandler::Get( ).updateItems( );
		trapsHandler::Get( ).updateTraps( );

		entitiesHandler::Get( ).updateEnemiesCollision( );

		levelManager.updateLevel( );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	Log::Print( "[updateEntities] bye!" );
}