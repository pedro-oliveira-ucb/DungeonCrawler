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
	CBaseEntityAnimationType animationType;
};

#define RAD2DEG 57.29577951308232f
#define DEG2RAD 0.01745329251994f // ou (PI / 180.0f)
// Normaliza um ângulo pra faixa [-180, +180]
float AngleDiff( float a , float b ) {
	float diff = fmodf( a - b + 180.0f , 360.0f );
	if ( diff < 0 ) diff += 360.0f;
	return diff - 180.0f;
}

// Função para calcular o ângulo entre dois pontos em radianos
double calcularAnguloRad( GVector2D origem , GVector2D destino ) {
	GVector2D dAngle = destino - origem;
	return atan2( dAngle.y , dAngle.x ); // Retorna ângulo em radianos
}

// Função para converter radianos em graus
double radParaGraus( double rad ) {
	return rad * 180.0 / M_PI;
}

CBaseEntityAnimationType getAnimationTypeBasedOnStateAndDirection( CBaseEntityState entityState, DIRECTION entityDirection ) {
	switch ( entityState ) {
	case CBaseEntityState::MOVING:
		switch ( entityDirection ) {
		case DIRECTION::FORWARD:
			return WALKING_FORWARD;
		case DIRECTION::BACKWARD:
			return WALKING_BACKWARD;
		case DIRECTION::LEFT:
			return WALKING_LEFT;
		case DIRECTION::RIGHT:
			return WALKING_RIGHT;
		default:
			return IDLE_FORWARD; // Padrão
		}
	case CBaseEntityState::STOPPED:
		switch ( entityDirection ) {
		case DIRECTION::FORWARD:
			return IDLE_FORWARD;
		case DIRECTION::BACKWARD:
			return IDLE_BACKWARD;
		case DIRECTION::LEFT:
			return IDLE_LEFT;
		case DIRECTION::RIGHT:
			return IDLE_RIGHT;
		default:
			return IDLE_FORWARD; // Padrão
		}
	case CBaseEntityState::ATTACKING:
		switch ( entityDirection ) {
		case DIRECTION::FORWARD:
			return ATTACKING_FORWARD;
		case DIRECTION::BACKWARD:
			return ATTACKING_BACKWARD;
		case DIRECTION::LEFT:
			return ATTACKING_LEFT;
		case DIRECTION::RIGHT:
			return ATTACKING_RIGHT;
		default:
			return ATTACKING_FORWARD; // Padrão
		}

	default:
		return IDLE_FORWARD; // Padrão
	}
}



void updatePositionEvent::threadFunction( ) {
	Log::Print( "[updatePositionEvent] Hello world!" );
	keybindHandler::Get( ).initializeKeybinds( );
	CPlayerEntity * localPlayer = _gameWorld.localplayer;

	std::vector<movementUpdater> data = {
		movementUpdater( MOVE_FORWARD, MOVEMENT_FORWARD, WALKING_FORWARD ),
		movementUpdater( MOVE_BACKWARD, MOVEMENT_BACKWARD, WALKING_BACKWARD ),
		movementUpdater( MOVE_RIGHT, MOVEMENT_RIGHT, WALKING_RIGHT ),
		movementUpdater( MOVE_LEFT, MOVEMENT_LEFT, WALKING_LEFT )
	};

	while ( true ) {
		if ( !localPlayer ) {
			Log::Print( "[updatePositionEvent] Local player is null" );
			continue;
		}
	
		Log::Print( "Looking: %f" , localPlayer->getLookingAngle( ).getDegrees( ) );
		Log::Print( "Movement Angle: %f" , localPlayer->getMovementAngle( ) );

	
		GVector2D localPos = localPlayer->getEntityPosition( );
		bool reverse = false;
		bool playerMoving = false;
		for ( int i = 0; i < data.size( ); i++ ) {
			movementUpdater move = data.at( i );
			if ( keybindHandler::Get( ).isPressed( move.key ) ) {
				playerMoving = true;
				// Define a direção do movimento
				localPlayer->addMoveRequest( move.movementDirection );
				float delta = AngleDiff( localPlayer->getLookingAngle( ).getDegrees( ) , localPlayer->getMovementAngle( ) );
				reverse = ( fabsf( delta ) > 90.0f );

			}
		}

		bool playerAttacking = false;
		if ( keybindHandler::Get( ).isPressed( keybind_identifier::ATTACK_SIMPLE ) ) {
			playerAttacking = true;
		}

		float lookingAngle = localPlayer->getLookingAngle( ).getDegrees( );
		DIRECTION localDirection = DIRECTION::LEFT;

		if ( lookingAngle > -45 && lookingAngle <= 45 )
			localDirection = DIRECTION::RIGHT;
		else if ( lookingAngle > 45 && lookingAngle <= 135 )
			localDirection = DIRECTION::BACKWARD;
		else if ( lookingAngle > 135 || lookingAngle <= -135 )
			localDirection = DIRECTION::LEFT;
		else
			localDirection = DIRECTION::FORWARD;

		CBaseEntityState updateState = localPlayer->getEntityState( );

		if ( !playerAttacking ) {
			if ( !playerMoving )
				updateState = CBaseEntityState::STOPPED;
			else {
				updateState = CBaseEntityState::MOVING;				
			}
		}
		else {
			updateState = CBaseEntityState::ATTACKING;
		}

		switch ( updateState ) {
		case  CBaseEntityState::ATTACKING:
			reverse = false;
			localPlayer->clearMovementRequest( );
			break;
		case CBaseEntityState::MOVING:
			localPlayer->move( );
			break;

		case CBaseEntityState::STOPPED:

			break;
		}

		CBaseEntityAnimationType updateAnimation = getAnimationTypeBasedOnStateAndDirection(updateState, localDirection);

		float newLookingAngle = radParaGraus( calcularAnguloRad( localPos , GVector2D( globals.mousePosX , globals.mousePosY ) ) );
		localPlayer->setLookingAngle( newLookingAngle );
		localPlayer->setEntityLookingDirection( localDirection );
		localPlayer->setEntityState( updateState );

		localPlayer->getEntityAnimations( )->SetCurrentAnimation( updateAnimation );
		localPlayer->getEntityAnimations( )->GetCurrentAnimation( )->updateAnimation( true , reverse );


		if ( keybindHandler::Get( ).isPressed( UPDATE_MENU ) )
			globals.updateWindow = true;

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}