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
	std::mutex musicMutex;

	std::unordered_map<musicType , std::vector<std::pair<std::string, std::unique_ptr<rMusic>>>> musics;
	int currentPlayingSound = -1;
	musicType currentMusicType = MainMenuMusic;
	musicType oldMusicType = NoMusic;

	float volume = 1.0f;
	float fadeSpeed = 1.0f; // Volume por segundo
	bool onMusicTransition = false;
	bool isFadingOut = false;
	bool isFadingIn = false;

	rMusic * currentSound = nullptr;
	rMusic * nextSound = nullptr;

public:
	rMusicManager( std::string path ) :rBaseResource( path ) { }
	rMusicManager( const rMusicManager & ) = delete;
	rMusicManager & operator=( const rMusicManager & ) = delete;

	// Optionally delete move operations
	rMusicManager( rMusicManager && ) = delete;
	rMusicManager & operator=( rMusicManager && ) = delete;

	bool initialize( ) override;
	bool playMusic( musicType musicType );
	void updateMusic( );
};

