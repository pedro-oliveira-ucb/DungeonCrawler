#include "gameState.h"

#include "../../../Globals/Globals.h"

#include <raylib/raylib.h>



gameStateTransitionState gameState::onStateFadeIn( float deltaTime )
{
	float alphaIncrementThisFrame = ( 255.0f / this->transitionDuration ) * deltaTime;

	alpha += alphaIncrementThisFrame;

	// Clamp the alpha value to a maximum of 255.0f.
	if ( alpha > 255.0f ) {
		alpha = 255.0f;
		this->exiting = false;
		return gameStateTransitionState::EXIT_FINISHED;
	}

	return gameStateTransitionState::EXIT_IN_PROGRESS;
}

gameStateTransitionState gameState::onStateFadeOut( float deltaTime )
{
	float alphaIncrementThisFrame = ( 255.0f / this->transitionDuration ) * deltaTime;

	alpha -= alphaIncrementThisFrame;

	if ( alpha < 0 ) {
		alpha = 0;
		this->entering = false;
		return gameStateTransitionState::ENTER_FINISHED;
	}

	return gameStateTransitionState::ENTER_IN_PROGRESS;
}

void gameState::renderTransition( gameStateManager * manager ) {
	// Prepare the color for the fade rectangle.
	// Alpha component is cast to unsigned char, as typically expected by color structures.
	Color col = { 0, 0, 0, static_cast< unsigned char >( alpha ) };

	// Draw the fade rectangle.
	DrawRectangle( 0 , 0 , Globals::Get( ).screenWidth , Globals::Get( ).screenHeight , col );
}

gameStateTransitionState gameState::updateStateTransition( float deltaTime ) {

	if ( entering ) {
		return onStateFadeOut( deltaTime );
	}
	else if ( exiting ) {
		return onStateFadeIn( deltaTime );
	}

	return gameStateTransitionState::NO_TRANSITION;
}

void gameState::setEntering( bool value ) {
	entering = value;
}

void gameState::setExiting( bool value ) {
	exiting = value;
}