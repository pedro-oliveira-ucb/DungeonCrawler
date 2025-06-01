#pragma once
#include <mutex>

enum currentGameStateType {
	GAME_STATE_NONE,
	GAME_STATE_PLAYING,
	GAME_STATE_PAUSED,
	GAME_STATE_EXIT
};

class currentGameState
{
	mutable std::mutex currentGameStateMutex;

	currentGameStateType gameState = GAME_STATE_NONE;

public:

	void setCurrentGameState( currentGameStateType newState );
	bool isGameinState( currentGameStateType state ) const;
	currentGameStateType getCurrentGameState( ) const;	

};

