#pragma once
#include <chrono>
#include "gameSettings/gameSettings.h"
#include "currentGameState/currentGameState.h"

#include "../Utils/singleton.h"

#define now std::chrono::high_resolution_clock::now( )

class Globals : public CSingleton<Globals>
{
	gameSettings settings;
	currentGameState state;

public:

	int screenWidth = 1920;
	int screenHeight = 1080;

	gameSettings * getGameSettings( );
	currentGameState * getCurrentGameState( );

	bool updateWindow = false;
	bool windowInitialized = false;
	bool resourcesLoaded = false;

	float mousePosX;
	float mousePosY;

	float mousePosWorldX;
	float mousePosWorldY;
};


