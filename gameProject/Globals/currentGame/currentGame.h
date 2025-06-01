#pragma once
#include <mutex>

enum currentGameState {
	GAME_STATE_NONE,
	GAME_STATE_PLAYING,
	GAME_STATE_PAUSED,
	GAME_STATE_EXIT
};

class currentGame
{
	mutable std::mutex currentGameStateMutex;

	currentGameState gameState = GAME_STATE_NONE;
	double currentGameTime = 0.0f;
public:
	void setCurrentGameState( currentGameState newState );
	bool isGameinState( currentGameState state ) const;
	currentGameState getCurrentGameState( ) const;
	void updateCurrentGameTime( double deltaTime );
	double getCurrentGameTime( ) const;

};

