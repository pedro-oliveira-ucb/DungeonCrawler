#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <vector>

#include "../../../gameResources/gameResource/rMusic/rMusic.h"
#include "../../../gameResources/rBaseResource/rBaseResource.h"

enum musicType
{
	NoMusic = -1 ,
	MainMenuMusic = 0 ,
	DungeonMusic = 1 ,
	BossMusic = 2 ,
};

class rMusicManager : public rBaseResource
{
	mutable std::mutex musicMutex;

	std::unordered_map<musicType , std::vector<std::pair<std::string, std::unique_ptr<rMusic>>>> musics;
	int currentPlayingSound = -1;
	musicType currentMusicType = NoMusic;
	musicType oldMusicType = NoMusic;

	float originalBaseVolume = 1.0f;
	float currentBaseVolume = 1.0f;
	float volume = 1.0f;
	float fadeSpeed = 1.0f; // Volume por segundo
	bool onMusicTransition = false;
	bool isFadingOut = false;
	bool isFadingIn = false;

	rMusic * currentSound = nullptr;
	rMusic * nextSound = nullptr;
	int currentSoundIndex = -1; 

public:
	rMusicManager( std::string path ) :rBaseResource( path ) { }
	rMusicManager( const rMusicManager & ) = delete;
	rMusicManager & operator=( const rMusicManager & ) = delete;

	// Optionally delete move operations
	rMusicManager( rMusicManager && ) = delete;
	rMusicManager & operator=( rMusicManager && ) = delete;

	musicType getcurrentMusicType( ) const;
	bool initialize( ) override;
	bool playMusic( musicType musicType, float fadeSpeed );
	void updateMusic( );
	void setMusicVolume( float volume );
	void pauseMusic( );
	void resumeMusic( );
};

