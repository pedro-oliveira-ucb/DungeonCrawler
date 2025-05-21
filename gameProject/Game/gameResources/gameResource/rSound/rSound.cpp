#include "rSound.h"
#include <raylib/raylib.h>
#include <fstream>
#include <vector>

#include "../../../Utils/Log/Log.h"

rSound::rSound( std::string model )
	: soundPath( model ) , soundAddress( nullptr ) {
	// Lazy loading opcional: 
	initializeSound( );
}

rSound::~rSound( ) {
	if ( soundAddress ) {
		UnloadSound( *( Sound * ) soundAddress );
		delete ( Sound * ) soundAddress;
		soundAddress = nullptr;
	}

}

void rSound::initializeSound( ) {
	if ( !soundAddress ) {
		Sound * sound = new Sound( );
		// Load the sound from the buffer
		*sound = LoadSound( soundPath.c_str( ) );
		soundAddress = static_cast< void * >( sound );
		Log::Print( "Loaded %s, address: 0x%p" , soundPath.c_str( ) , soundAddress );
	}
}

void rSound::playSound( ) {
	if ( !soundAddress ) {
		initializeSound( );
	}
	// Play the sound
	PlaySound( *( Sound * ) soundAddress );
}

void * rSound::getSound( ) {
	if ( !soundAddress ) {
		initializeSound( );
	}
	return soundAddress;
}
