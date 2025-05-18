#include "updatePosition.h"

#include "../../gameControls/keybindHandler/keybindHandler.h"
#include "../../gameControls/mouseMovement/mouseMovement.h"

#include "../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"
#include "../../gameWorld/gameWorld.h"



struct  movementUpdater {
	keybind_identifier key;
	CBaseEntityMovementDirection movementDirection;
	CBaseEntityAnimationType animationType;
};


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

	movementUpdater lastMove = data.at( 0 );
	mouseAngle mAngle = mouseMovement::Get( ).getMouseAngle( );
	while ( true ) {
		if ( !localPlayer ) {
			Log::Print( "[updatePositionEvent] Local player is null" );
			continue;
		}

		mAngle = mouseMovement::Get( ).getMouseAngle( );
		Log::Print( "mSin: %f, mCos: %f" , mAngle.sin , mAngle.cos );

		bool playerMoving = false;
		for ( int i = 0; i < data.size( ); i++ ) {
			movementUpdater move = data.at( i );
			if ( keybindHandler::Get( ).isPressed( move.key ) ) {
				lastMove = move;
				//Set the entity movement direction
				localPlayer->MoveEntity( move.movementDirection );
				//Current entity, state, we're pressing moving key, so the current state is moving
				localPlayer->setEntityState( CBaseEntityState::MOVING );
				//Chance entity animation sprites
				localPlayer->getEntityAnimations( )->SetCurrentAnimation( move.animationType );
				playerMoving = true;
			}
		}

		if ( !playerMoving ) {
			switch ( lastMove.movementDirection ) {
			case CBaseEntityMovementDirection::MOVEMENT_FORWARD:
				localPlayer->getEntityAnimations( )->SetCurrentAnimation( IDLE_FORWARD );
				break;
			case CBaseEntityMovementDirection::MOVEMENT_BACKWARD:
				localPlayer->getEntityAnimations( )->SetCurrentAnimation( IDLE_BACKWARD );
				break;
			case CBaseEntityMovementDirection::MOVEMENT_LEFT:
				localPlayer->getEntityAnimations( )->SetCurrentAnimation( IDLE_LEFT );
				break;
			case CBaseEntityMovementDirection::MOVEMENT_RIGHT:
				localPlayer->getEntityAnimations( )->SetCurrentAnimation( IDLE_RIGHT );
				break;
			}

			localPlayer->setEntityState( CBaseEntityState::STOPPED );
		}

		CBaseEntityState localPlayerState = localPlayer->getEntityState( );
		CBaseEntityAnimationType localPlayerAnimationType = localPlayer->getEntityAnimations( )->getCurrentAnimationType( );
		Log::Print( "Current anim: %d" , localPlayerAnimationType );

		localPlayer->getEntityAnimations( )->GetCurrentAnimation( )->updateAnimation( );
		Log::Print( "Animation Address: %p" , localPlayer->getEntityAnimations( )->GetCurrentAnimation( ) );

		Log::Print( "Updated animation" );
		//Log::Print( "Current animation: %s" , localPlayer->getEntityAnimations( )->GetCurrentAnimation( )->getModelName( ).c_str( ) );

		if ( keybindHandler::Get( ).isPressed( UPDATE_MENU ) )
			globals.updateWindow = true;

		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}