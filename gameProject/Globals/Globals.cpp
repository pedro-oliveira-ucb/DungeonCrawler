#include "Globals.h"

gameSettings * Globals::getGameSettings( )
{
	return &this->settings;
}

currentGameState * Globals::getCurrentGameState( )
{
	return &this->state;
}