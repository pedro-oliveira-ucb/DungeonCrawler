#pragma once
#include <mutex>

#include "../../Game/SDK/math/Vector2D/GVector2D.h"

enum currentGameState {
	GAME_STATE_NONE,
	GAME_STATE_PLAYING,
	GAME_STATE_PAUSED,
	GAME_STATE_GAME_OVER,
	GAME_STATE_EXIT
};

class currentGame
{
	mutable std::mutex currentGameStateMutex;

	currentGameState gameState = GAME_STATE_NONE;
	double currentGameTime = 0.0f;
	int currentGameRoom = 0;
	int currentGameLevel = 0;
	GVector2D currentLocalPlayerPosition = GVector2D( 0.0f , 0.0f );

	int numKeys = 0;
	int numMana = 0.f;
public:
	void setCurrentGameState( currentGameState newState );
	bool isGameinState( currentGameState state ) const;
	currentGameState getCurrentGameState( ) const;
	void updateCurrentGameTime( double deltaTime );
	double getCurrentGameTime( ) const;
	int getCurrentGameRoom( ) const;
	void setCurrentGameRoom( int room );
	void setCurrentGameLevel( int level );
	int getCurrentGameLevel( ) const;

	GVector2D getCurrentLocalPlayerPosition( ) const;
	void setCurrentLocalPlayerPosition( const GVector2D & position );

	void addKey( int quantity );
	void removeKey( int quantity );
	int getNumKeys( ) const;	

	void addMana( int quantity );
	void removeMana( int quantity );
	int getNumMana( ) const;	
};

