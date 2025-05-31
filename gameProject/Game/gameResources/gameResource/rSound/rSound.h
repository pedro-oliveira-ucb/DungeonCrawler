#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <memory>


struct Sound;

#define MAX_SOUNDS 255

struct SoundConfig {
	float volume = 1.0f; // Volume de 0.0f a 1.0f
	float pitch = 1.0f; // Pitch do som (1.0f = normal)
	int maxInstances = 255; // Número máximo de instâncias do som que podem ser reproduzidas simultaneamente
	static bool generateSoundConfig( std::string filename , SoundConfig * buffer );
	static bool createBaseSoundConfig( std::string filename );
};

class rSound
{
	std::mutex soundMutex;
	bool initialized = false;
	float baseVolume = 1.0f;

	std::string soundPath;
	std::vector<std::unique_ptr<Sound>> sounds;
	void initializeSound( );
public:
	rSound( const rSound & ) = delete;
	rSound & operator=( const rSound & ) = delete;

	rSound( std::string model, SoundConfig config );
	~rSound( );

	SoundConfig config;
	void setVolume( float volume );
	void playSound( );
	void pauseSounds( );
	void setVolumePercentage( float volume );
};
