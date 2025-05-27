#pragma once
#include <string>
#include <mutex> // Adicionado para thread safety

struct Music; // Forward declaration, definido em raylib

enum class MusicTransitionState {
	None ,
	FadeIn ,
	FadeOut
};

struct MusicConfig {
	float volume = 1.0f; // Volume de 0.0f a 1.0f
	bool loop = true;
};

class rMusic {
private:
	Music * music = nullptr;
	std::string path;
	MusicConfig config;
	float currentVolume = 0.0f;
	float fadeDuration = 1.0f;
	float fadeTimer = 0.0f;
	MusicTransitionState transitionState = MusicTransitionState::None;
	bool initialized = false;


	mutable std::mutex mtx; // Mutex para proteger membros compartilhados

public:
	rMusic( std::string filePath , MusicConfig config );
	~rMusic( );

	void initialize( );
	void play( float fadeInTime = 0.0f );
	void stop( float fadeOutTime = 0.0f );
	void update( float deltaTime );

	float getBaseVolume( );
	void setVolume( float volume );
	bool isPlaying( ) const;
	bool isFading( ) const;
};