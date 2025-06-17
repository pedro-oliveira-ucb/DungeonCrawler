#include "updateLocalPlayer.h"

#include <cmath>

#include "../../gameControls/keybindHandler/keybindHandler.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"



void updateLocalPlayer::threadFunction( ) {
	Log::Print( "[updateLocalPlayer] Hello world!" );
	keybindHandler::Get( ).initializeKeybinds( );

	while ( true ) {
		currentGameState gameState = Globals::Get( ).getGame( )->getCurrentGameState( );

		if ( gameState != currentGameState::GAME_STATE_PLAYING ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
			continue;
		}

		entitiesHandler::Get( ).updateLocalPlayer( );

		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	Log::Print( "[updateLocalPlayer] bye!" );
}