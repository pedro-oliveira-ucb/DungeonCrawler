#include "Globals.h"

gameSettings * Globals::getGameSettings( )
{
	return &this->settings;
}

currentGame * Globals::getGame( )
{
	return &this->game;
}

playerStats * Globals::getPlayerStats( )
{
	return &this->stats;
}

float Globals::getFrameTime( ) const
{
	std::lock_guard<std::mutex> lock( this->globalsMutex );
	return this->frameTime;
}

void Globals::setFrameTime( float frameTime )
{
	std::lock_guard<std::mutex> lock( this->globalsMutex );
	this->frameTime = frameTime;
}


void * Globals::getDefaultFont( ) {
	return this->defaultFont;
}

void Globals::setDefaultFont( void * font ) {
	this->defaultFont = font;
}