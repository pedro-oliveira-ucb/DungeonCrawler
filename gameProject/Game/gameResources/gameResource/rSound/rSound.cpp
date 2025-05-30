#include "rSound.h"


#include <fstream>
#include <vector>
#include <thread>

#include "../../../../Utils/Log/Log.h"
#include <raylib/raylib.h>

#include <filesystem>

namespace fs = std::filesystem;

rSound::rSound( std::string model, SoundConfig config )
	: soundPath( model ), config(config){
	// Lazy loading opcional: 
	initializeSound( );
}

rSound::~rSound( ) {
	if ( initialized ) {
	
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

		if ( !fs::is_regular_file( path ) ) {
			Log::Print( "Error: Provided path is not a regular file: %s" , path.string( ).c_str( ) );
			return;
		}

		auto mainSound = std::make_unique<Sound>( LoadSound( soundPath.c_str( ) ) );
		SetSoundVolume( *mainSound , config.volume );
		SetSoundPitch( *mainSound , config.pitch );

		this->sounds.emplace_back( std::move( mainSound ) );

		for ( int i = 0; i < config.maxInstances; i++ ) {
			auto aliasSound = std::make_unique<Sound>( LoadSoundAlias( *this->sounds.front( ).get( ) ) );
			SetSoundVolume( *aliasSound , config.volume );
			SetSoundPitch( *aliasSound , config.pitch );
			this->sounds.emplace_back( std::move( aliasSound ) );
		}

		this->initialized = true;
	}
}

void rSound::playSound( ) {
	std::lock_guard<std::mutex> lock( soundMutex );
	if ( !initialized ) {
		initializeSound( );
	}

	if ( this->sounds.empty( ) ) {
		Log::Print( "Error: Sound buffer is empty" );
		return;
	}

	// Procura um som disponível (não está tocando)
	for ( auto & soundPtr : this->sounds ) {
		if ( !IsSoundPlaying( *soundPtr ) ) {
			PlaySound( *soundPtr );
			return;
		}
	}

	Log::Print( "Warning: All sound instances are currently playing, skipping playback" );
}