#include "rSoundsManager.h"


#include "../../../Utils/Log/Log.h"
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

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
		this->loadSound( animationPath );
	}

	Log::Print( "[rSoundsManager] Initialized %d sounds!\nAvailable sounds:" , this->sounds.size( ) );

	for ( const auto & it : this->sounds ) {
		Log::Print( "[rSoundsManager] [%s] sound" , it.first.c_str( ) );
	}

	return true;
}

void rSoundsManager::loadSound( std::string name ) {
	fs::path clipPath = name;

	if ( clipPath.empty( ) ) {
		Log::Print( "[rSpritesManager] Failed to load clip %s, path is empty!" , name.c_str( ) );
		return;
	}

	if ( !fs::exists( clipPath ) || !fs::is_directory( clipPath ) ) {
		Log::Print( "[rSpritesManager] Folder not found: %s" , clipPath.string( ).c_str( ) );
		return;
	}

	std::string soundFilePath;

	for ( const auto & entry : fs::directory_iterator( clipPath ) ) {
		if ( !entry.is_regular_file( ) ) continue;
		const std::string filename = entry.path( ).string( );
		soundFilePath = filename;
		break;
	}

	if ( soundFilePath.empty( ) ) {
		return;
	}

	fs::path basePath = this->getPath( );
	std::string relativePath = fs::relative( clipPath , basePath ).string( );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );

	rSound * sound = new rSound( soundFilePath );
	if ( sound == nullptr )
		return;

	std::shared_ptr<rSound> soundPtr( sound );

	this->sounds.emplace( std::make_pair( relativePath.c_str( ) , soundPtr ) );
}

bool rSoundsManager::playSound( std::string  name ) {
	if ( this->sounds.find( name ) != this->sounds.end( ) ) {
		this->sounds.at( name )->playSound( );
		return true;
	}

	return false;
}
