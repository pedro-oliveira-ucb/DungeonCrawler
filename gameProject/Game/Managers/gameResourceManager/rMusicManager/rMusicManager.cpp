#include "rMusicManager.h"

#include <filesystem>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>

#include "./../../../../Globals/Globals.h"

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
		Log::Print( "[rMusicManager] Failed to create config file: %s" , filename.c_str( ) );
		return false;
	}

	file << config.dump( 4 );

	file.close( );
	Log::Print( "[rMusicManager] Created base music config file: %s" , filename.c_str( ) );
}

bool MusicConfig::generateMusicConfig( std::string filename , MusicConfig * buffer ) {
	if ( !fs::exists( filename ) ) {
		Log::Print( "[rMusicManager] Config file not found: %s" , filename.c_str( ) );
		if ( !createBaseMusicConfig( filename ) ) {
			Log::Print( "[rMusicManager] Failed to create base music config file: %s" , filename.c_str( ) );
			return false;
		}
	}

	std::string configString = utils::Get( ).readFileAsString( filename );
	if ( configString.empty( ) ) {
		Log::Print( "[rMusicManager] Config file is empty: %s" , filename.c_str( ) );
		return false;
	}

	json configJson;
	try {
		configJson = json::parse( configString );
	}
	catch ( const json::parse_error & e ) {
		Log::Print( "[rMusicManager] Failed to parse config file %s: %s" , filename.c_str( ) , e.what( ) );
		return false;
	}

	std::vector<std::string> requiredKeys = { "volume", "loop" };

	for ( const auto & key : requiredKeys ) {
		if ( configJson.find( key ) == configJson.end( ) ) {
			Log::Print( "[rMusicManager] Config file %s does not contain '%s' key!" , filename.c_str( ) , key.c_str( ) );
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
	pausedMusic = true;

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
	pausedMusic = false;

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

	static const std::unordered_map<std::string , musicType> musicTypeMap = {
	{ "dungeon", musicType::DungeonMusic },
	{ "boss", musicType::BossMusic },
	{ "menu", musicType::MainMenuMusic },
	{ "gameover", musicType::gameOverMusic }
	};

	for ( auto & files : FilesOnFolder ) {
		std::vector<std::pair<int , std::string>> orderedFiles; // Para ordenar os arquivos por nome (0.png, 1.png, ...)

		for ( auto & file : files ) {
			fs::path configPath = file.folderPath + "\\config.json";

			MusicConfig musicConfig;

			if ( !MusicConfig::generateMusicConfig( configPath.string( ) , &musicConfig ) ) {
				Log::Print( "[rMusicManager] Failed to generate music config for %s!" , file.fileName.c_str( ) );
				return false;
			}

			musicType type = musicType::MainMenuMusic;
			bool foundType = false;

			for ( const auto & [keyword , mType] : musicTypeMap ) {
				if ( file.recursiveFileName.find( keyword ) != std::string::npos ) {
					type = mType;
					foundType = true;
					break;
				}
			}

			if ( !foundType ) {
				Log::Print( "[rMusicManager] Unknown music type for %s!" , file.recursiveFileName.c_str( ) );
				return false;
			}

			musics[ type ].emplace_back( std::make_pair( file.recursiveFileName , std::make_unique<rMusic>( file.filePath , musicConfig ) ) );
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

	if ( pausedMusic )
		return false;

	// Se já estiver em transição, armazena a nova solicitação para depois
	if ( onMusicTransition ) {
		hasPendingTransition = true;
		pendingType = newType;
		pendingSpeed = speed;
		return true;
	}

	if ( newType != currentMusicType ) {
		oldMusicType = currentMusicType;
		currentSoundIndex = 0;
	}

	auto it = musics.find( newType );
	if ( it == musics.end( ) || it->second.empty( ) ) {
		Log::Print( "[rMusicManager] No music found for type %d!" , static_cast< int >( newType ) );
		return false;
	}

	currentMusicType = newType;

	if ( it->second.size( ) > 1 ) {
		int tempCurrentIndex = currentSoundIndex;
		do {
			tempCurrentIndex = utils::Get( ).randomNumber( 0 , static_cast< int >( it->second.size( ) - 1 ) );
		} while ( tempCurrentIndex == currentSoundIndex );
		currentSoundIndex = tempCurrentIndex;
	}
	else {
		currentSoundIndex = 0;
	}

	nextSound = it->second.at( currentSoundIndex ).second.get( );
	isFadingOut = true;
	isFadingIn = false;
	onMusicTransition = true;

	if ( currentBaseVolume > 0.0f && speed > 0.0f ) {
		fadeSpeed = ( currentBaseVolume + nextSound->getBaseVolume( ) ) / speed;
	}
	else if ( speed > 0.0f ) {
		fadeSpeed = nextSound->getBaseVolume( ) / speed;
	}
	else {
		if ( currentSound ) {
			currentSound->stop( );
			currentSound->setVolume( 0.0f );
		}
		currentSound = nextSound;
		nextSound = nullptr;
		if ( currentSound ) {
			currentSound->play( );
			volume = currentSound->getBaseVolume( );
			currentSound->setVolume( volume );
		}
		onMusicTransition = false;
		isFadingIn = false;
		isFadingOut = false;
	}

	return true;
}

void rMusicManager::updateMusic( ) {
	float delta = Globals::Get( ).getFrameTime( );
	float currentSoundBaseVolume = 1.0f;
	bool needImmediateTransition = false;
	musicType immediateType = currentMusicType;
	float immediateSpeed = 5.0f;


	{   // Primeiro bloco: update da faixa atual
		std::lock_guard<std::mutex> guard( this->musicMutex );
		if ( currentSound ) {
			// musica perto do fim, sinaliza transição imediata
			if ( currentSound->update( delta ) ) {
				needImmediateTransition = true;
				immediateType = currentMusicType;
				immediateSpeed = 5.0f;
			}
			currentSoundBaseVolume = currentSound->getBaseVolume( );
		}
	}

	{   // Segundo bloco: gerencia fade-in / fade-out
		std::lock_guard<std::mutex> lock( this->musicMutex );
		if ( onMusicTransition ) {
			if ( isFadingOut ) {
				volume -= fadeSpeed * delta;
				if ( volume <= 0.0f ) {
					volume = 0.0f;
					isFadingOut = false;
					if ( currentSound ) 
						currentSound->stop( );

					currentSound = nextSound;
					nextSound = nullptr;
					if ( currentSound ) 
						currentSound->play( );
					isFadingIn = true;
				}
				if ( currentSound ) currentSound->setVolume( volume );
			}
			else if ( isFadingIn ) {
				volume += fadeSpeed * delta;
				if ( volume >= currentSoundBaseVolume ) {
					volume = currentSoundBaseVolume;
					isFadingIn = false;
					onMusicTransition = false;

					// sinaliza que há transição pendente
					if ( hasPendingTransition ) {
						hasPendingTransition = false;
						needImmediateTransition = true;
						immediateType = pendingType;
						immediateSpeed = pendingSpeed;
					}
				}
				if ( currentSound ) currentSound->setVolume( volume );
			}
		}
	}

	// Só agora, **depois** de liberar o mutex, fazemos a transição
	if ( needImmediateTransition ) {
		playMusic( immediateType , immediateSpeed );
	}
}