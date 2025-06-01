#include "rMusicManager.h"

#include <filesystem>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>

#include "./../../../../Utils/Log/Log.h"
#include "./../../../../Utils/utils.h"

#include <nlohmann/json.hpp>
#include <raylib/raylib.h>


namespace fs = std::filesystem;
using nlohmann::json;

bool MusicConfig::createBaseMusicConfig( std::string filename ) {

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

bool MusicConfig::generateMusicConfig( std::string filename , MusicConfig * buffer ) {
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


void rMusicManager::setMusicVolume( float volume ) {
	std::lock_guard<std::mutex> lock( this->musicMutex );

	for ( const auto & it : this->musics ) {
		for ( int i = 0; i < it.second.size( ); ++i ) {
			it.second.at( i ).second->setVolumePercentage( volume );
		}
	}
}

void rMusicManager::pauseMusic( ) {
	std::lock_guard<std::mutex> lock( this->musicMutex );

	for ( const auto & it : this->musics ) {
		for ( int i = 0; i < it.second.size( ); ++i ) {
			if ( it.second.at( i ).second->isPlaying( ) ) {
				it.second.at( i ).second->pause( );
			}
		}
	}
}

void rMusicManager::resumeMusic( ) {
	std::lock_guard<std::mutex> lock( this->musicMutex );

	for ( const auto & it : this->musics ) {
		for ( int i = 0; i < it.second.size( ); ++i ) {
			if ( it.second.at( i ).second->isPaused( ) ) {
				it.second.at( i ).second->resume( );
			}
		}
	}
}

musicType rMusicManager::getcurrentMusicType( ) const
{
	std::lock_guard<std::mutex> lock( this->musicMutex );
	return this->currentMusicType;
}

bool rMusicManager::initialize( )
{
	std::vector<std::vector<fileScanResult>> FilesOnFolder = this->recursiveGetFiles( this->getPath( ) , ".wav" );

	if ( FilesOnFolder.empty( ) ) {
		Log::Print( "[rMusicManager] No files found in %s" , this->getPath( ).c_str( ) );
		return false;
	}

	for ( auto & files : FilesOnFolder ) {
		std::vector<std::pair<int , std::string>> orderedFiles; // Para ordenar os arquivos por nome (0.png, 1.png, ...)

		for ( auto & file : files ) {
			fs::path configPath = file.folderPath + "\\config.json";

			MusicConfig musicConfig;

			if ( !MusicConfig::generateMusicConfig( configPath.string( ) , &musicConfig ) ) {
				Log::Print( "[rMusicManager] Failed to generate sound config for %s!" , file.fileName.c_str( ) );
				return false;
			}

			musicType type = musicType::MainMenuMusic; // Default type, can be set later if needed

			if ( file.rawRecursivePath.find( "dungeon" ) != std::string::npos ) {
				type = musicType::DungeonMusic;
			}
			else if ( file.rawRecursivePath.find( "boss" ) != std::string::npos ) {
				type = musicType::BossMusic;
			}
			else if ( file.rawRecursivePath.find( "menu" ) != std::string::npos ) {
				type = musicType::MainMenuMusic;
			}
			else {
				Log::Print( "[rMusicManager] Unknown music type for %s!" , file.rawRecursivePath.c_str( ) );
				return false;
			}

			musics[ type ].emplace_back( std::make_pair( file.rawRecursivePath , std::make_unique<rMusic>( file.filePath , musicConfig ) ) );
			break;
		}
	}

	Log::Print( "[rMusicManager] Initialized %d musics!\nAvailable sounds:" , this->musics.size( ) );


	for ( const auto & it : this->musics ) {
		for ( int i = 0; i < it.second.size( ); ++i ) {
			Log::Print( "[rMusicManager] [%s] music" , it.second.at( i ).first.c_str( ) );
		}
	}

	return true;
}

bool rMusicManager::playMusic( musicType newType , float speed ) {
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
	if ( currentBaseVolume > 0.0f && speed > 0.0f ) { // Verifica se há música antiga e speed é válido
		fadeSpeed = ( currentBaseVolume + nextSound->getBaseVolume( ) ) / speed;
	}
	else if ( speed > 0.0f ) { // Nenhuma música antiga, apenas fade in
		fadeSpeed = nextSound->getBaseVolume( ) / speed;
	}
	else {
		if ( currentSound ) {
			currentSound->stop( );
			currentSound->setVolume( 0.0f ); // Garante que o som antigo pare e fique mudo
		}
		currentSound = nextSound;
		nextSound = nullptr;
		if ( currentSound ) {
			currentSound->play( );
			this->volume = currentSound->getBaseVolume( ); // Ajusta o volume principal da classe
			currentSound->setVolume( this->volume );      // Ajusta o volume do objeto de som
		}
		onMusicTransition = false;
		isFadingIn = false;
		isFadingOut = false;
	}

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