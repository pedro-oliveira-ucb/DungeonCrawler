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

	static bool createBaseMusicConfig( std::string filename );
	static bool generateMusicConfig( std::string filename , MusicConfig * buffer );
};

class rMusic {
private:
	Music * music = nullptr;
	std::string path;
	float baseVolume = 1.0f;
	MusicConfig config;
	float currentVolume = 0.0f;
	float fadeDuration = 1.0f;
	float fadeTimer = 0.0f;
	MusicTransitionState transitionState = MusicTransitionState::None;
	bool initialized = false;


	mutable std::mutex mtx; // Mutex para proteger membros compartilhados

	bool paused = false;

public:
	rMusic( std::string filePath , MusicConfig config );
	~rMusic( );

	void initialize( );
	void setVolumePercentage( float percentage );
	void play( float fadeInTime = 0.0f );
	void stop( float fadeOutTime = 0.0f );
	void pause( );
	void resume( );
	void update( float deltaTime );

	float getBaseVolume( );
	void setVolume( float volume );
	bool isPlaying( ) const;
	bool isFading( ) const;
	bool isPaused( ) const;
};