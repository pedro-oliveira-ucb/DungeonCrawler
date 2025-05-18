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
    GVector2D dAngle  = destino - origem;
    return atan2( dAngle.y , dAngle.x ); // Retorna ângulo em radianos
}

// Função para converter radianos em graus
double radParaGraus( double rad ) {
    return rad * 180.0 / M_PI;
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

    movementUpdater lastMove = data.at( 0 );

    while ( true ) {
        if ( !localPlayer ) {
            Log::Print( "[updatePositionEvent] Local player is null" );
            continue;
        }
		GVector2D localPos = localPlayer->getEntityPosition( );
   
		float angle = radParaGraus(calcularAnguloRad( localPos , GVector2D(globals.mousePosX, globals.mousePosY) ));
        localPlayer->setLookingAngle( angle );

        Log::Print( "Looking: %f" , localPlayer->getLookingAngle( ).getDegrees() );
        Log::Print( "Movement Angle: %f" , localPlayer->getMovementAngle() );

        bool reverse = false;
        bool playerMoving = false;
        for ( int i = 0; i < data.size( ); i++ ) {
            movementUpdater move = data.at( i );
            if ( keybindHandler::Get( ).isPressed( move.key ) ) {
                lastMove = move;
                playerMoving = true;

                // Define a direção do movimento
                localPlayer->addMoveRequest( move.movementDirection );
                localPlayer->setEntityState( CBaseEntityState::MOVING );

        
                float delta = AngleDiff( localPlayer->getLookingAngle().getDegrees() , localPlayer->getMovementAngle( ) );
                reverse = ( fabsf( delta ) > 90.0f );
            }
        }

        localPlayer->move( );

        if ( !playerMoving ) { 
            localPlayer->setEntityState( CBaseEntityState::STOPPED );
        }

        // Define animação de idle com base no ângulo de visão
        float lookingAngle = localPlayer->getLookingAngle( ).getDegrees( );
        CBaseEntityAnimationType currentAnimation;

        if ( lookingAngle > -45 && lookingAngle <= 45 )
            currentAnimation = playerMoving? WALKING_RIGHT : IDLE_RIGHT;
        else if ( lookingAngle > 45 && lookingAngle <= 135 )
            currentAnimation = playerMoving ?WALKING_BACKWARD : IDLE_BACKWARD;
        else if ( lookingAngle > 135 || lookingAngle <= -135 )
            currentAnimation = playerMoving ? WALKING_LEFT : IDLE_LEFT;
        else
            currentAnimation = playerMoving ? WALKING_FORWARD : IDLE_FORWARD;

        localPlayer->getEntityAnimations( )->SetCurrentAnimation( currentAnimation );

        CBaseEntityState localPlayerState = localPlayer->getEntityState( );
        CBaseEntityAnimationType localPlayerAnimationType = localPlayer->getEntityAnimations( )->getCurrentAnimationType( );
        Log::Print( "Current anim: %d" , localPlayerAnimationType );

        localPlayer->getEntityAnimations( )->GetCurrentAnimation( )->updateAnimation(true, reverse );
        if ( keybindHandler::Get( ).isPressed( UPDATE_MENU ) )
            globals.updateWindow = true;

        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    }
}