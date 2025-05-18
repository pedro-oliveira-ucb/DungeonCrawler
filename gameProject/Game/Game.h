#pragma once
class Game
{
	bool resourcesLoaded = false;
	bool threadsStarted = false;

public:
	bool start( );
	bool isResourcesLoaded( );
};

