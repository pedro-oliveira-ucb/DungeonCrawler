#include "rSoundsManager.h"

#include <filesystem>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>

#include "../../../../Utils/utils.h"

#include "../../../../Utils/Log/Log.h"
#include <nlohmann/json.hpp>



namespace fs = std::filesystem;

using nlohmann::json;


bool SoundConfig::createBaseSoundConfig( std::string filename ) {

	json config;

	config[ "volume" ] = 1.0f;
	config[ "pitch" ] = 1.0f;
	config[ "maxInstances" ] = 255;

	std::ofstream file( filename );
	if ( !file.is_open( ) ) {
		Log::Print( "[rSpritesManager] Failed to create config file: %s" , filename.c_str( ) );
		return false;
	}

	file << config.dump( 4 );

	file.close( );
	Log::Print( "[rSpritesManager] Created base sound config file: %s" , filename.c_str( ) );
}

bool SoundConfig::generateSoundConfig( std::string filename , SoundConfig * buffer ) {

	if ( !fs::exists( filename ) ) {
		Log::Print( "[rSpritesManager] Config file not found: %s" , filename.c_str( ) );
		if ( !createBaseSoundConfig( filename ) ) {
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

	std::vector<std::string> requiredKeys = { "volume", "pitch", "maxInstances" };

	for ( const auto & key : requiredKeys ) {
		if ( configJson.find( key ) == configJson.end( ) ) {
			Log::Print( "[rSpritesManager] Config file %s does not contain '%s' key!" , filename.c_str( ) , key.c_str( ) );
			return false;
		}
	}

	buffer->maxInstances = configJson[ "maxInstances" ].get<int>( );
	buffer->pitch = configJson[ "pitch" ].get<float>( );
	buffer->volume = configJson[ "volume" ].get<float>( );

	return true;
}

void rSoundsManager::setVolume( float volume ) {
	std::lock_guard<std::mutex>( this->mtx );
	for ( const auto & it : this->sounds ) {
		it.second->setVolumePercentage( volume );
	}
}

bool rSoundsManager::initialize( )
{
	std::vector<std::vector<fileScanResult>> FilesOnFolder = this->recursiveGetFiles( this->getPath( ) , ".wav" );

	if ( FilesOnFolder.empty( ) ) {
		Log::Print( "[rSoundsManager] No files found in %s" , this->getPath( ).c_str( ) );
		return false;
	}

	for ( auto & files : FilesOnFolder ) {
		std::vector<std::pair<int , std::string>> orderedFiles; // Para ordenar os arquivos por nome (0.png, 1.png, ...)

		for ( auto & file : files ) {
			fs::path configPath = file.folderPath + "\\config.json";

			SoundConfig soundConfig;

			if ( !SoundConfig::generateSoundConfig( configPath.string( ) , &soundConfig ) ) {
				Log::Print( "[rSoundsManager] Failed to generate sound config for %s!" , file.fileName.c_str( ) );
				return false;
			}

			std::unique_ptr<rSound> soundPtr = std::make_unique<rSound>( file.filePath , soundConfig );
			this->sounds.emplace( std::make_pair( file.rawRecursivePath , std::move( soundPtr ) ) );
			//just one sound per folder, so we can break after the first match
			break;
		}
	}

	Log::Print( "[rSoundsManager] Initialized %d sounds!\nAvailable sounds:" , this->sounds.size( ) );

	for ( const auto & it : this->sounds ) {
		Log::Print( "[rSoundsManager] [%s] sound" , it.first.c_str( ) );
	}

	return true;
}

bool rSoundsManager::playSound( std::string  name ) {
	if ( this->sounds.find( name ) != this->sounds.end( ) ) {
		this->sounds.at( name )->playSound( );
		return true;
	}

	return false;
}
