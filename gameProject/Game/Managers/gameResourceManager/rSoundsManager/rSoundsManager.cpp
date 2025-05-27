#include "rSoundsManager.h"

#include <filesystem>
#include <regex>
#include <fstream>
#include <sstream>
#include <string>

#include "../../../../Utils/utils.h"

#include "../../../Utils/Log/Log.h"
#include <nlohmann/json.hpp>



namespace fs = std::filesystem;

using nlohmann::json;


bool createBaseSoundConfig( std::string filename ) {

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

bool generateSoundConfig( std::string filename , SoundConfig * buffer ) {

	if ( !fs::exists( filename ) ) {
		Log::Print( "[rSpritesManager] Config file not found: %s" , filename.c_str( ) );
		if ( !createBaseSoundConfig( filename ) ) {
			Log::Print( "[rSpritesManager] Failed to create base sound config file: %s" , filename.c_str( ) );
			return false;
		}
	}

	std::string configString = utils::Get().readFileAsString( filename );
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


bool rSoundsManager::initialize( )
{
	fs::path basePath = this->getPath( );

	if ( !fs::exists( basePath ) ) {
		Log::Print( "[rSoundsManager] %s doesn't exist!" , basePath.string( ).c_str( ) );
		return false;
	}

	Log::Print( "[rSoundsManager] Recursively scanning folders in: %s" , basePath.string( ).c_str( ) );

	std::regex soundFileRegex( R"(sound\.wav)" , std::regex::icase );

	std::vector<std::string> toLoad;

	for ( const auto & entry : fs::recursive_directory_iterator( basePath ) ) {
		if ( !fs::is_directory( entry ) ) continue;

		bool containsSound = false;

		// verifica se a pasta tem arquivos tipo 0.png, 1.png, etc.
		for ( const auto & file : fs::directory_iterator( entry ) ) {
			if ( fs::is_regular_file( file ) ) {
				std::string filename = file.path( ).filename( ).string( );
				if ( std::regex_match( filename , soundFileRegex ) ) {
					containsSound = true;
					break;
				}
			}
		}

		if ( containsSound ) {
			// gera chave tipo localplayer_idle_backward
			Log::Print( "[rSoundsManager] Found sprite animation folder: %s" , entry.path( ).string( ).c_str( ) );
			toLoad.emplace_back( entry.path( ).string( ).c_str( ) );
		}
	}

	for ( auto animationPath : toLoad ) {
		Log::Print( "[rSoundsManager] Loading %s!" , animationPath.c_str( ) );
		if ( !this->loadSound( animationPath ) ) {
			Log::Print( "[rSoundsManager] Failed to load sound %s!" , animationPath.c_str( ) );
			return false;
		}
		else {
			Log::Print( "[rSoundsManager] Successfully loaded sound %s!" , animationPath.c_str( ) );
		}
	}

	Log::Print( "[rSoundsManager] Initialized %d sounds!\nAvailable sounds:" , this->sounds.size( ) );

	for ( const auto & it : this->sounds ) {
		Log::Print( "[rSoundsManager] [%s] sound" , it.first.c_str( ) );
	}

	return true;
}


bool rSoundsManager::loadSound( std::string name ) {
	fs::path clipPath = name;
	fs::path configPath = name + "\\config.json";

	if ( clipPath.empty( ) ) {
		Log::Print( "[rSpritesManager] Failed to load clip %s, path is empty!" , name.c_str( ) );
		return false;
	}

	if ( !fs::exists( clipPath ) || !fs::is_directory( clipPath ) ) {
		Log::Print( "[rSpritesManager] Folder not found: %s" , clipPath.string( ).c_str( ) );
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
		Log::Print( "[rSpritesManager] No sound file found in %s!" , clipPath.string( ).c_str( ) );
		return false;
	}

	fs::path basePath = this->getPath( );
	std::string relativePath = fs::relative( clipPath , basePath ).string( );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );

	SoundConfig soundConfig;

	if ( !generateSoundConfig( configPath.string( ) , &soundConfig ) ) {
		Log::Print( "[rSpritesManager] Failed to generate sound config for %s!" , name.c_str( ) );
		return false;
	}

	std::unique_ptr<rSound> soundPtr = std::make_unique<rSound>( soundFilePath , soundConfig );
	this->sounds.emplace( std::make_pair( relativePath , std::move( soundPtr ) ) );

	return true;
}

bool rSoundsManager::playSound( std::string  name ) {
	if ( this->sounds.find( name ) != this->sounds.end( ) ) {
		this->sounds.at( name )->playSound( );
		return true;
	}

	return false;
}
