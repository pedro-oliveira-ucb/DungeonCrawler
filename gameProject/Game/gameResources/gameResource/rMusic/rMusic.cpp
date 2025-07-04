#include "rMusic.h"
#include "../../../../Utils/Log/Log.h"

#include <raylib/raylib.h>
#include <filesystem>
#include <algorithm> // Para std::clamp

namespace fs = std::filesystem;

rMusic::rMusic( std::string filePath , MusicConfig  cfg )
	: path( filePath ) {
	this->baseVolume = cfg.volume;
	this->config.volume = cfg.volume;
	this->config.loop = cfg.loop;
	initialize( );
}

rMusic::~rMusic( ) {
	if ( initialized && music ) {
		StopMusicStream( *music );
		UnloadMusicStream( *music );
		delete music;
	}
}

void rMusic::initialize( ) {
	std::lock_guard<std::mutex> lock( mtx );
	if ( initialized ) return;

	if ( !fs::exists( path ) ) {
		Log::Print( "rMusic: Music file does not exist: %s" , path.c_str( ) );
		return;
	}

	music = new Music( LoadMusicStream( path.c_str( ) ) );
	music->looping = config.loop;
	SetMusicVolume( *music , 0.0f );
	initialized = true;
}

void rMusic::setVolumePercentage( float percentage ) {
	std::lock_guard<std::mutex> lock( mtx );
	percentage = std::clamp( percentage , 0.0f , 100.0f ); // Ensure percentage is between 0 and 100
	float desiredVolume = this->baseVolume * ( percentage / 100.0f );
	config.volume = desiredVolume;
}

void rMusic::play( float fadeInTime ) {
	std::lock_guard<std::mutex> lock( mtx );
	if ( !initialized ) initialize( );

	PlayMusicStream( *music );
	if ( fadeInTime > 0.0f ) {
		transitionState = MusicTransitionState::FadeIn;
		fadeTimer = 0.0f;
		fadeDuration = fadeInTime;
		currentVolume = 0.0f;
	}
	else {
		currentVolume = config.volume;
		SetMusicVolume( *music , config.volume );
	}
}

void rMusic::stop( float fadeOutTime ) {
	std::lock_guard<std::mutex> lock( mtx );
	if ( fadeOutTime > 0.0f && isPlaying( ) ) {
		transitionState = MusicTransitionState::FadeOut;
		fadeTimer = 0.0f;
		fadeDuration = fadeOutTime;
	}
	else {
		StopMusicStream( *music );
	}
}

void rMusic::pause( ) {
	std::lock_guard<std::mutex> lock( mtx );
	PauseMusicStream( *music );
	this->paused = true;
}

void rMusic::resume( ) {
	std::lock_guard<std::mutex> lock( mtx );
	ResumeMusicStream( *music );
	this->paused = false;
}

bool rMusic::isPaused( ) const {
	std::lock_guard<std::mutex> lock( mtx );
	return this->paused;
}

bool rMusic::update( float deltaTime ) {
	if ( !initialized || !isPlaying( ) )
		return false;

	std::lock_guard<std::mutex> lock( mtx );

	UpdateMusicStream( *music );

	if ( transitionState == MusicTransitionState::FadeIn ) {
		fadeTimer += deltaTime;
		currentVolume = ( fadeTimer / fadeDuration ) * config.volume;
		if ( fadeTimer >= fadeDuration ) {
			currentVolume = config.volume;
			transitionState = MusicTransitionState::None;
		}
		SetMusicVolume( *music , currentVolume );
	}
	else if ( transitionState == MusicTransitionState::FadeOut ) {
		fadeTimer += deltaTime;
		currentVolume = config.volume * ( 1.0f - ( fadeTimer / fadeDuration ) );
		if ( fadeTimer >= fadeDuration ) {
			currentVolume = 0.0f;
			transitionState = MusicTransitionState::None;
			StopMusicStream( *music );
		}
		SetMusicVolume( *music , currentVolume );
	}
	else {
		SetMusicVolume( *music , config.volume );
	}

	bool musicPlaying = IsMusicStreamPlaying( *music );
	float musicTimePlayed = GetMusicTimePlayed( *music );
	float musicLength = GetMusicTimeLength( *music );

	return musicPlaying && musicTimePlayed > ( musicLength - 5.0f );
}

void rMusic::setVolume( float volume ) {
	std::lock_guard<std::mutex> lock( mtx );
	//config.volume = volume;
	if ( transitionState == MusicTransitionState::None && music ) {
		currentVolume = volume;
		SetMusicVolume( *music , volume );
	}
}

float rMusic::getBaseVolume( ) {
	std::lock_guard<std::mutex> lock( mtx );
	return config.volume;
}

bool rMusic::isPlaying( ) const {
	std::lock_guard<std::mutex> lock( mtx );
	return music && IsMusicStreamPlaying( *music );
}

bool rMusic::isFading( ) const {
	std::lock_guard<std::mutex> lock( mtx );
	return transitionState != MusicTransitionState::None;
}