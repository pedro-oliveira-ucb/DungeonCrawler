#pragma once
class Globals
{
public:
	int screenWidth = 1920;
	int screenHeight = 1080;


	bool exitGame = false;
	bool updateWindow = false;
	bool windowInitialized = false;
	bool resourcesLoaded = false;

	float mousePosX;
	float mousePosY;

	void * resourceManagerPointer = nullptr;
	void * windowPointer = nullptr;

	float mouseWheelDelta = 0.0f; // <-- Adicionado para armazenar o scroll atual
};

extern Globals globals;

