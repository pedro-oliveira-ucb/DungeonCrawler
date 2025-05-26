#pragma once
#include <mutex>
#include <vector>
#include <string>
#include <memory>
#include <mutex>

class Sound;

#define MAX_SOUNDS 255

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

	rSound( std::string model );
	~rSound( );

	void playSound( );
};
