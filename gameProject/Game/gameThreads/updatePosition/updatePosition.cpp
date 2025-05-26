#include "updatePosition.h"

#include <cmath>

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../gameObjects/entitiesHandler/entitiesHandler.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

struct movementUpdater {
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
	CPlayerEntity * localPlayer = entitiesHandler::Get( ).getLocalPlayer( );

	std::vector<movementUpdater> data = {
		{ MOVE_FORWARD,   MOVEMENT_FORWARD },
		{ MOVE_BACKWARD,  MOVEMENT_BACKWARD },
		{ MOVE_RIGHT,     MOVEMENT_RIGHT },
		{ MOVE_LEFT,      MOVEMENT_LEFT }
	};

	while ( true ) {
		if ( !localPlayer ) {
			Log::Print( "[updatePositionEvent] Local player is null" );
			continue;
		}

		if ( GetAsyncKeyState( VK_DELETE ) ) {
			localPlayer->Hit( 50 );
		}

		if ( GetAsyncKeyState( VK_HOME ) ) {
			localPlayer->Respawn( );
		}



		if ( GetAsyncKeyState( VK_SHIFT ) ) {
			localPlayer->setSprinting(true );
		}else
			localPlayer->setSprinting( false );

		GVector2D localPos = localPlayer->getEntityPosition( );
		float newLookingAngle = radParaGraus( calcularAnguloRad( localPos , GVector2D( globals.mousePosX , globals.mousePosY ) ) );

		// Ataques do jogador
		if ( keybindHandler::Get( ).isPressed( keybind_identifier::ATTACK_SIMPLE ) ) {
			localPlayer->UseAttack( CBaseAttackType_Melee );
		}
		if ( keybindHandler::Get( ).isPressed( keybind_identifier::ATTACK_HEAVY ) ) {
			localPlayer->UseAttack( CBaseAttackType_Ranged );
		}
		localPlayer->setLookingAngle( newLookingAngle );

		// Movimento do player
		for ( auto move : data ) {
			if ( keybindHandler::Get( ).isPressed( move.key ) ) {
				localPlayer->addMoveRequest( move.movementDirection  );
			}
		}

		// Atualiza player
		localPlayer->updateEntity( );

		std::this_thread::sleep_for( std::chrono::milliseconds( 20 ) );
	}
}