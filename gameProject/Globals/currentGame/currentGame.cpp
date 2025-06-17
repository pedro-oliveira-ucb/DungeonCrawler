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

bool currentGame::isInCorridor( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->inCorridor;
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

void currentGame::setInCorridor( bool inCorridor )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->inCorridor = inCorridor;
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

void currentGame::addKey( int quantity )
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->numKeys += quantity;
}

void currentGame::removeKey( int quantity )
{
	this->numKeys -= quantity;
	// Ensure numKeys does not go below zero
	this->numKeys = std::max( 0 , this->numKeys );
}

int currentGame::getNumKeys( ) const
{
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->numKeys;
}

void currentGame::addMana( int quantity ) {
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->numMana += quantity;
}

void currentGame::removeMana( int quantity ) {
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	this->numMana -= quantity;
	// Ensure numMana does not go below zero
	this->numMana = std::max( 0 , this->numMana );
}

int currentGame::getNumMana( ) const {
	std::lock_guard<std::mutex> lock( currentGameStateMutex );
	return this->numMana;
}