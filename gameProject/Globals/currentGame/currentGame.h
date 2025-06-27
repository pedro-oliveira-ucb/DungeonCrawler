#pragma once
#include <mutex>

#include <unordered_map>

#include "../../Game/SDK/math/Vector2D/GVector2D.h"

enum currentGameState {
	GAME_STATE_NONE,
	GAME_STATE_PLAYING,
	GAME_STATE_PAUSED,
	GAME_STATE_GAME_OVER,
	GAME_STATE_EXIT
};

enum CBaseAttackType;

class currentGame
{
	mutable std::mutex currentGameStateMutex;

	currentGameState gameState = GAME_STATE_NONE;
	double currentGameTime = 0.0f;
	int currentGameRoom = 0;
	int currentGameLevel = 0;
	bool inCorridor = false;	
	float localPlayerHealthPercentage = 1.0f; 
	GVector2D currentLocalPlayerPosition = GVector2D( 0.0f , 0.0f );

	std::unordered_map<CBaseAttackType , float> attackTypeCooldowns;
	
	int numKeys = 0;
	int numMana = 100000.f;
public:
	void setCurrentGameState( currentGameState newState );
	bool isGameinState( currentGameState state ) const;
	bool isInCorridor( ) const;	
	currentGameState getCurrentGameState( ) const;
	void updateCurrentGameTime( double deltaTime );
	double getCurrentGameTime( ) const;
	int getCurrentGameRoom( ) const;
	void setCurrentGameRoom( int room );
	void setCurrentGameLevel( int level );
	void setInCorridor( bool inCorridor );
	int getCurrentGameLevel( ) const;
	float getLocalPlayerHealthPercentage( ) const;

	void setAttackTypeCooldown( CBaseAttackType attackType , float cooldown );
	float getAttackTypeCooldown( CBaseAttackType attackType ) const;


	GVector2D getCurrentLocalPlayerPosition( ) const;
	void setCurrentLocalPlayerPosition( const GVector2D & position );
	void setLocalPlayerHealthPercentage( float );

	void addKey( int quantity );
	void removeKey( int quantity );
	int getNumKeys( ) const;	

	void addMana( int quantity );
	void removeMana( int quantity );
	int getNumMana( ) const;	
};

