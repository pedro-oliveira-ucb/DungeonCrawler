#pragma once
#include <mutex>


class gameSettings
{
	mutable std::mutex gameSettingsMutex;

	float soundVolume = 80.0f;
	float musicVolume = 80.0f;
public:

	void setSoundVolume( float volume );
	float getSoundVolume( ) const;

	void setMusicVolume( float volume );
	float getMusicVolume( ) const;
	
};

