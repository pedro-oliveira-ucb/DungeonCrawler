#include "gameSettings.h"

void gameSettings::setMusicVolume(float volume) {
	std::lock_guard<std::mutex> lock( this->gameSettingsMutex );
	this->musicVolume = volume;
}

void gameSettings::setSoundVolume( float volume ) {
	std::lock_guard<std::mutex> lock( this->gameSettingsMutex );
	this->soundVolume = volume;
}

float gameSettings::getMusicVolume( )const {
	std::lock_guard<std::mutex> lock( this->gameSettingsMutex );
	return this->musicVolume;
}

float gameSettings::getSoundVolume( )const {
	std::lock_guard<std::mutex> lock( this->gameSettingsMutex );
	return this->soundVolume;
}