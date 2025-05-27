#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <memory>
#include <mutex>

class Sound;

#define MAX_SOUNDS 255

struct SoundConfig {
	float volume = 1.0f; // Volume de 0.0f a 1.0f
	float pitch = 1.0f; // Pitch do som (1.0f = normal)
	int maxInstances = 255; // Número máximo de instâncias do som que podem ser reproduzidas simultaneamente
};

class rSound
{
	std::mutex soundMutex;
	bool initialized = false;

	std::string soundPath;
	std::vector<std::unique_ptr<Sound>> sounds;
	void initializeSound( );
public:
	rSound( const rSound & ) = delete;
	rSound & operator=( const rSound & ) = delete;

	rSound( std::string model, SoundConfig config );
	~rSound( );

	SoundConfig config;
	void playSound( );
};
