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

int currentGame::getCurrentGameRoom( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->currentGameRoom;
}

void currentGame::setCurrentGameRoom( int room )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->currentGameRoom = room;
}

void currentGame::setCurrentGameLevel( int level )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->currentGameLevel = level;
}

int currentGame::getCurrentGameLevel( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->currentGameLevel;
}

GVector2D currentGame::getCurrentLocalPlayerPosition( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->currentLocalPlayerPosition;
}

void currentGame::setCurrentLocalPlayerPosition( const GVector2D & position )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->currentLocalPlayerPosition = position;
}
