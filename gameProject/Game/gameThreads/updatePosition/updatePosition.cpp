#include "updatePosition.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"
#include "../../gameWorld/gameWorld.h"

#include <cmath>

struct  movementUpdater {
	keybind_identifier key;
	CBaseEntityMovementDirection movementDirection;
};

double calcularAnguloRad( GVector2D origem , GVector2D destino ) {
	GVector2D dAngle = destino - origem;
	return atan2( dAngle.y , dAngle.x ); 
}

double radParaGraus( double rad ) {
	return rad * 180.0 / M_PI;
}

void updatePositionEvent::threadFunction( ) {
	Log::Print( "[updatePositionEvent] Hello world!" );
	keybindHandler::Get( ).initializeKeybinds( );
	CPlayerEntity * localPlayer = _gameWorld.localplayer;

	std::vector<movementUpdater> data = {
		movementUpdater( MOVE_FORWARD, MOVEMENT_FORWARD ),
		movementUpdater( MOVE_BACKWARD, MOVEMENT_BACKWARD ),
		movementUpdater( MOVE_RIGHT, MOVEMENT_RIGHT ),
		movementUpdater( MOVE_LEFT, MOVEMENT_LEFT )
	};

	while ( true ) {

	

		if ( !localPlayer ) {
			Log::Print( "[updatePositionEvent] Local player is null" );
			continue;
		}
	
		GVector2D localPos = localPlayer->getEntityPosition( );
		float newLookingAngle = radParaGraus( calcularAnguloRad( localPos , GVector2D( globals.mousePosX , globals.mousePosY ) ) );

		// if ( !localPlayer->isAttacking( ) ) {
			if ( keybindHandler::Get( ).isPressed( keybind_identifier::ATTACK_SIMPLE ) ) {
				localPlayer->UseAttack( 0 );
			}

			if ( keybindHandler::Get( ).isPressed( keybind_identifier::ATTACK_HEAVY ) ) {
				localPlayer->UseAttack( 1 );
			}

			localPlayer->setLookingAngle( newLookingAngle );

			for ( int i = 0; i < data.size( ); i++ ) {
				movementUpdater move = data.at( i );
				if ( keybindHandler::Get( ).isPressed( move.key ) ) {
					// Define a direção do movimento
					localPlayer->addMoveRequest( move.movementDirection );
				}
			}
		//}

		localPlayer->updateEntity( );

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}