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

	Log::Print( "[updateLevel] Hello world, my id:%d " , ( int ) ( this->ThreadObject->GetId( ) ) );

	CPlayerEntity * local = entitiesHandler::Get( ).getLocalPlayer( );
	int currentPlayerRoom = -1;

	bool spawnedKey = false;

	while ( true ) {
		currentGameState gameState = Globals::Get( ).getGame( )->getCurrentGameState( );

		if ( gameState != currentGameState::GAME_STATE_PLAYING ) {
			std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
			continue;
		}

		if ( local == nullptr ) {
			continue;
		}

		GVector2D localPos = Globals::Get( ).getGame( )->getCurrentLocalPlayerPosition( );

		int localRoom = gameMap::Get( ).getRoomIdAtPosition( localPos );

		Globals::Get( ).getGame( )->setCurrentGameRoom( localRoom );
		
		if ( keybindHandler::Get( ).isPressed( keybind_identifier::UPDATE_MENU ) ) {
			levelManager.moveToNextLevel( );
		}

		bool inCorridor = gameMap::Get( ).isCorridor( localPos );
		Globals::Get( ).getGame( )->setInCorridor( inCorridor );
		if ( !inCorridor ) {
			if ( localRoom ) {
				if ( currentPlayerRoom == -1 ) {
					if ( local != nullptr ) {
						currentPlayerRoom = localRoom;
					}
				}

				if ( !levelManager.hasAliveEnemyOnCurrentRoom( localRoom ) ) {
					int roomsAmount = levelManager.roomsInCurrentLevel( );
					if ( localRoom >= roomsAmount ) {
						Log::Print( "[updateLevel] Finished level" , std::to_string( localRoom ).c_str( ) );
						levelManager.moveToNextLevel( );
					}
					else if ( !spawnedKey ) {
						spawnedKey = true;
						itemsHandler::Get( ).spawnItem( ItemType::KEY_ITEM );
						// Se n�o houver inimigos vivos, dropa a chave
						Log::Print( "[updateLevel] No enemies alive in room %s, dropping key" , std::to_string( localRoom ).c_str( ) );
					}
				}
				else {
					spawnedKey = false;
				}
			}
		}

		levelManager.update( );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	Log::Print( "[updateLevel] bye!" );
}