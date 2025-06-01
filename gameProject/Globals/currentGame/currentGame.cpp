#include "currentGame.h"

void currentGame::setCurrentGameState( currentGameState newState )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->gameState = newState;
}

bool currentGame::isGameinState( currentGameState state ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->gameState == state;
}

currentGameState currentGame::getCurrentGameState( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->gameState;
}

void currentGame::updateCurrentGameTime( double deltaTime )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->currentGameTime += deltaTime;
}

double currentGame::getCurrentGameTime( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->currentGameTime;
}
