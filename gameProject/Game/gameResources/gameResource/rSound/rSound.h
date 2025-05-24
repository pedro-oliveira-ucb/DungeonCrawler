#pragma once
#include <mutex>
#include <vector>
#include <string>


class rSound
{
	bool initialized = false;

	std::string soundPath;
	void * soundAddress; // tipo opaco
	void initializeSound( );
	

public:
	rSound( const rSound & ) = delete;
	rSound & operator=( const rSound & ) = delete;

	rSound( std::string model );
	~rSound( );

	void playSound( );
};
