#include "currentGameState.h"

void currentGameState::setCurrentGameState( currentGameStateType newState )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->gameState = newState;
}

bool currentGameState::isGameinState( currentGameStateType state ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->gameState == state;
}

currentGameStateType currentGameState::getCurrentGameState( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->gameState;
}
