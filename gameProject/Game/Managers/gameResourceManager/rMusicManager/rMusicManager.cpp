#include "rMusicManager.h"

#include <filesystem>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>

#include "../../../Utils/Log/Log.h"
#include "../../../Utils/utils.h"

#include <nlohmann/json.hpp>
#include <raylib/raylib.h>


namespace fs = std::filesystem;
using nlohmann::json;

bool createBaseMusicConfig( std::string filename ) {

	json config;

	config[ "volume" ] = 1.0f;
	config[ "loop" ] = false;

	std::ofstream file( filename );
	if ( !file.is_open( ) ) {
		Log::Print( "[rSpritesManager] Failed to create config file: %s" , filename.c_str( ) );
		return false;
	}

	file << config.dump( 4 );

	file.close( );
	Log::Print( "[rSpritesManager] Created base sound config file: %s" , filename.c_str( ) );
}

bool generateMusicConfig( std::string filename , MusicConfig * buffer ) {

	if ( !fs::exists( filename ) ) {
		Log::Print( "[rSpritesManager] Config file not found: %s" , filename.c_str( ) );
		if ( !createBaseMusicConfig( filename ) ) {
			Log::Print( "[rSpritesManager] Failed to create base sound config file: %s" , filename.c_str( ) );
			return false;
		}
	}

	std::string configString = utils::Get( ).readFileAsString( filename );
	if ( configString.empty( ) ) {
		Log::Print( "[rSpritesManager] Config file is empty: %s" , filename.c_str( ) );
		return false;
	}

	json configJson;
	try {
		configJson = json::parse( configString );
	}
	catch ( const json::parse_error & e ) {
		Log::Print( "[rSpritesManager] Failed to parse config file %s: %s" , filename.c_str( ) , e.what( ) );
		return false;
	}

	std::vector<std::string> requiredKeys = { "volume", "loop" };

	for ( const auto & key : requiredKeys ) {
		if ( configJson.find( key ) == configJson.end( ) ) {
			Log::Print( "[rSpritesManager] Config file %s does not contain '%s' key!" , filename.c_str( ) , key.c_str( ) );
			return false;
		}
	}

	buffer->loop = configJson[ "loop" ].get<bool>( );
	buffer->volume = configJson[ "volume" ].get<float>( );

	return true;
}

bool rMusicManager::initialize( )
{
	fs::path basePath = this->getPath( );

	if ( !fs::exists( basePath ) ) {
		Log::Print( "[rMusicManager] %s doesn't exist!" , basePath.string( ).c_str( ) );
		return false;
	}

	Log::Print( "[rMusicManager] Recursively scanning folders in: %s" , basePath.string( ).c_str( ) );

	std::vector<std::string> toLoad;

	for ( const auto & entry : fs::recursive_directory_iterator( basePath ) ) {
		if ( !fs::is_directory( entry ) ) continue;

		bool containsSound = false;

		// verifica se a pasta tem arquivos tipo 0.png, 1.png, etc.
		for ( const auto & file : fs::directory_iterator( entry ) ) {
			if ( fs::is_regular_file( file ) ) {
				std::string filename = file.path( ).filename( ).string( );
				if ( file.path( ).extension( ) != ".wav" )
					continue; // Only load .wav files
				containsSound = true;
				break;
			}
		}

		if ( containsSound ) {
			// gera chave tipo localplayer_idle_backward
			Log::Print( "[rMusicManager] Found sprite animation folder: %s" , entry.path( ).string( ).c_str( ) );
			toLoad.emplace_back( entry.path( ).string( ).c_str( ) );
		}
	}

	if ( toLoad.empty( ) ) {
		Log::Print( "[rMusicManager] No music folders found in %s!" , basePath.string( ).c_str( ) );
		return false;
	}

	for ( auto animationPath : toLoad ) {
		Log::Print( "[rMusicManager] Loading %s!" , animationPath.c_str( ) );
		if ( !this->loadSound( animationPath ) ) {
			Log::Print( "[rMusicManager] Failed to load sound %s!" , animationPath.c_str( ) );
			return false;
		}
		else {
			Log::Print( "[rMusicManager] Successfully loaded sound %s!" , animationPath.c_str( ) );
		}
	}

	Log::Print( "[rMusicManager] Initialized %d musics!" , this->musics.size( ) );

	return true;
}

bool rMusicManager::loadSound( std::string name ) {
	fs::path clipPath = name;
	fs::path configPath = name + "\\config.json";

	if ( clipPath.empty( ) ) {
		Log::Print( "[rMusicManager] Failed to load clip %s, path is empty!" , name.c_str( ) );
		return false;
	}

	if ( !fs::exists( clipPath ) || !fs::is_directory( clipPath ) ) {
		Log::Print( "[rMusicManager] Folder not found: %s" , clipPath.string( ).c_str( ) );
		return false;
	}

	std::string soundFilePath;
	for ( const auto & entry : fs::directory_iterator( clipPath ) ) {
		if ( !entry.is_regular_file( ) ) continue;
		if ( entry.path( ).extension( ) != ".wav" ) continue; // Only load .wav files
		const std::string filename = entry.path( ).string( );
		soundFilePath = filename;
		break;
	}

	if ( soundFilePath.empty( ) ) {
		Log::Print( "[rMusicManager] No sound file found in %s!" , clipPath.string( ).c_str( ) );
		return false;
	}

	fs::path basePath = this->getPath( );
	std::string relativePath = fs::relative( clipPath , basePath ).string( );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );

	MusicConfig musicConfig;

	if ( !generateMusicConfig( configPath.string( ) , &musicConfig ) ) {
		Log::Print( "[rMusicManager] Failed to generate sound config for %s!" , name.c_str( ) );
		return false;
	}

	musicType type = musicType::MainMenuMusic; // Default type, can be set later if needed

	if ( relativePath.find( "dungeon" ) != std::string::npos ) {
		type = musicType::DungeonMusic;
	}
	else if ( relativePath.find( "boss" ) != std::string::npos ) {
		type = musicType::BossMusic;
	}
	else if ( relativePath.find( "menu" ) != std::string::npos ) {
		type = musicType::MainMenuMusic;
	}
	else {
		Log::Print( "[rMusicManager] Unknown music type for %s!" , relativePath.c_str( ) );
		return false;
	}

	musics[ type ].emplace_back( std::make_pair( relativePath , std::make_unique<rMusic>( soundFilePath , musicConfig ) ) );
	return true;
}

bool rMusicManager::playMusic( musicType newType ) {
	std::lock_guard<std::mutex> lock( this->musicMutex );
	if ( onMusicTransition || newType == currentMusicType )
		return false;

	auto it = musics.find( newType );
	if ( it == musics.end( ) || it->second.empty( ) ) {
		Log::Print( "[rMusicManager] No music found for type %d!" , static_cast< int >( newType ) );
		return false;
	}

	oldMusicType = currentMusicType;
	currentMusicType = newType;

	nextSound = it->second.front( ).second.get( ); // Pega a primeira música do tipo
	isFadingOut = true;
	isFadingIn = false;
	onMusicTransition = true;

	return true;
}

void rMusicManager::updateMusic( ) {
	std::lock_guard<std::mutex> lock( this->musicMutex );
	float delta = GetFrameTime( );

	float currentSoundBaseVolume = 1.0f;

	if ( currentSound ) {
		currentSound->update( delta );
		currentSoundBaseVolume = currentSound->getBaseVolume( );
	}

	if ( onMusicTransition ) {
		if ( isFadingOut ) {
			volume -= fadeSpeed * delta;
			if ( volume <= 0.0f ) {
				volume = 0.0f;
				isFadingOut = false;

				if ( currentSound )
					currentSound->stop( ); // Presumido

				currentSound = nextSound;
				nextSound = nullptr;

				currentSound->play( );
				isFadingIn = true;
			}

			if ( currentSound )
				currentSound->setVolume( volume ); // Presumido
		}
		else if ( isFadingIn ) {
			volume += fadeSpeed * delta;
			if ( volume > currentSoundBaseVolume ) {
				volume = currentSoundBaseVolume;
				isFadingIn = false;
				onMusicTransition = false;
			}

			if ( currentSound )
				currentSound->setVolume( volume );
		}
	}
}