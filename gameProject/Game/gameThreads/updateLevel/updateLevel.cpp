#include "updateLevel.h"


#include "../../gameControls/keybindHandler/keybindHandler.h"

#include "../../gameObjects/gameMap/gameMap.h"

#include "../../Managers/collisionManager/collisionManager.h"
#include "../../Managers/LevelManager/LevelManager.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/attackHandler/attackHandler.h"
#include "../../Handlers/itemsHandler/itemsHandler.h"
#include "../../Handlers/trapsHandler/trapsHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

void updateLevel::threadFunction( ) {

	Log::Print( "[updateLevel] Hello world!" );

	CPlayerEntity * local = entitiesHandler::Get( ).getLocalPlayer( );
	int currentPlayerRoom = -1;

	while ( true ) {
		currentGameState gameState = Globals::Get( ).getGame( )->getCurrentGameState( );

		if ( gameState != currentGameState::GAME_STATE_PLAYING ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
			continue;
		}

		if ( local == nullptr ) {
			continue;
		}

		GVector2D localPos = local->getEntityPosition( );

		bool inCorridor = gameMap::Get( ).isCorridor( localPos );
		if ( !inCorridor ) {
			int localRoom = gameMap::Get( ).getRoomIdAtPosition( localPos );
			if ( localRoom ) {
				if ( currentPlayerRoom == -1 ) {
					if ( local != nullptr ) {
						currentPlayerRoom = localRoom;
					}
				}

				if ( !levelManager.hasAliveEnemyOnCurrentRoom( localRoom ) ) {
					// Se não houver inimigos vivos, dropa a chave
					Log::Print( "[updateLevel] No enemies alive in room %s, dropping key", std::to_string( localRoom ).c_str() );
				}
			}
		}

		levelManager.update( );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	Log::Print( "[updateLevel] bye!" );
}