#include "rSound.h"


#include <fstream>
#include <vector>
#include <thread>

#include "../../../Utils/Log/Log.h"

#include <SFML/Audio.hpp>

#include <filesystem>

namespace fs = std::filesystem;

rSound::rSound( std::string model )
	: soundPath( model ) {
	// Lazy loading opcional: 
	initializeSound( );
}

rSound::~rSound( ) {
	if ( initialized ) {
		delete static_cast< sf::SoundBuffer * >( soundAddress );
        this->soundAddress = nullptr;
	}
}
void rSound::initializeSound( ) {
    if ( !initialized ) {
        Log::Print( "Trying to load sound from path: %s" , soundPath.c_str( ) );

        if ( soundPath.empty( ) ) {
            Log::Print( "Error: Provided sound path is empty." );
            return;
        }

        std::filesystem::path path( soundPath );
    
        if ( !fs::exists( path ) ) {
            Log::Print( "Error: Sound file does not exist: %s" , path.string( ).c_str( ) );
            return;
        }

        sf::SoundBuffer * buffer = new sf::SoundBuffer( );
        sf::FileInputStream stream( path );

        if ( !buffer->loadFromStream( stream ) ) {
            Log::Print( "Error: Failed to load sound from: %s" , path.string( ).c_str( ) );
            throw std::runtime_error( "failed to load sound" );
            return;
        }

        this->soundAddress = static_cast< void * >( buffer );
        this->initialized = true;
    }
}



void rSound::playSound( ) {
	if ( !initialized ) {
		initializeSound( );
	}

	sf::SoundBuffer * buffer = static_cast< sf::SoundBuffer * > ( this->soundAddress );
	if(buffer == nullptr) {
		Log::Print( "Error: Sound buffer is null" );
		return;
	}

	sf::Sound som = sf::Sound( *buffer);
    som.play( );
}

