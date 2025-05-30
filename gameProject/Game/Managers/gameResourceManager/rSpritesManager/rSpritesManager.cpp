#include "rSpritesManager.h"
#include "..\..\..\..\Utils\Log\Log.h"
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

bool rSpritesManager::initialize( ) {
	fs::path basePath = this->getPath( );

	if ( !fs::exists( basePath ) ) {
		Log::Print( "[rSpritesManager] %s doesn't exist!" , basePath.string( ).c_str( ) );
		return false;
	}

	Log::Print( "[rSpritesManager] Recursively scanning folders in: %s" , basePath.string( ).c_str( ) );

	std::regex spriteFileRegex( R"(^\d+\.png$)" , std::regex::icase );

	std::vector<std::string> toLoad;

	for ( const auto & entry : fs::recursive_directory_iterator( basePath ) ) {
		if ( !fs::is_directory( entry ) ) continue;

		bool containsSprites = false;

		// verifica se a pasta tem arquivos tipo 0.png, 1.png, etc.
		for ( const auto & file : fs::directory_iterator( entry ) ) {
			if ( fs::is_regular_file( file ) ) {
				std::string filename = file.path( ).filename( ).string( );
				if ( std::regex_match( filename , spriteFileRegex ) ) {
					containsSprites = true;
					break;
				}
			}
		}

		if ( containsSprites ) {
			// gera chave tipo localplayer_idle_backward
			Log::Print( "[rSpritesManager] Found sprite animation folder: %s" , entry.path().string().c_str( ) );
			toLoad.emplace_back( entry.path( ).string( ).c_str( ) );
		}
	}

	for ( auto & animationPath : toLoad ) {
		Log::Print( "[rSpritesManager] Loading %s!" , animationPath.c_str( ) );
		this->loadClip( animationPath , SPRITE_TYPE::SINGULAR );
	}

	Log::Print( "[rSpritesManager] Initialized %d animations!\nAvailable animations:" , this->spriteAnimations.size( ) );

	for ( const auto & it : this->spriteAnimations ) {
		Log::Print( "[rSpritesManager] [%s] animation" , it.first.c_str( ) );
	}

	return true;
}


std::shared_ptr<rSpriteAnimation> rSpritesManager::loadClip( const std::string & name , SPRITE_TYPE type ) {
	fs::path clipPath = name;

	if ( clipPath.empty( ) ) {
		Log::Print( "[rSpritesManager] Failed to load clip %s, path is empty!" , name.c_str( ) );
		return nullptr;
	}

	if ( !fs::exists( clipPath ) || !fs::is_directory( clipPath ) ) {
		Log::Print( "[rSpritesManager] Folder not found: %s" , clipPath.string( ).c_str() );
		return nullptr;
	}

	std::vector<std::pair<int , std::string>> orderedFiles;

	// Regex para pegar arquivos com nome tipo 0.png, 1.png, ...
	std::regex pattern( R"((\d+)\.png)" );

	for ( const auto & entry : fs::directory_iterator( clipPath ) ) {
		if ( !entry.is_regular_file( ) ) continue;

		const std::string filename = entry.path( ).filename( ).string( );
		std::smatch match;
		if ( std::regex_match( filename , match , pattern ) ) {
			int index = std::stoi( match[ 1 ].str( ) );
			orderedFiles.emplace_back( index , entry.path( ).string( ) );
		}
	}

	// Ordena pelo índice (ordem crescente: 0.png, 1.png, 2.png, ...)
	std::sort( orderedFiles.begin( ) , orderedFiles.end( ) ,
		[ ] ( const auto & a , const auto & b ) {
			return a.first < b.first;
		} );


	std::vector<std::shared_ptr<rSprite>> sprites;
	// Cria os rSprites com os arquivos encontrados
	for ( const auto & [index , fullPath] : orderedFiles ) {
		Log::Print( "[rSpriteAnimation] Loading sprite: %s" , fullPath.c_str( ) );
		sprites.push_back( std::make_shared<rSprite>( fullPath ) );
	}
	availableSprites.push_back( std::move( sprites ) );

	fs::path basePath = this->getPath( );
	std::string relativePath = fs::relative( clipPath, basePath ).string( );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );

	rSpriteAnimation * animation = new rSpriteAnimation( availableSprites.back( ) , 24 );
	std::shared_ptr< rSpriteAnimation> sharedAnimationPtr( animation );

	this->spriteAnimations.emplace( relativePath.c_str() , sharedAnimationPtr );

	Log::Print( "[rSpritesManager] Loaded %d sprites" , static_cast< int >( sprites.size( ) ) );
}

std::shared_ptr<rSpriteAnimation> rSpritesManager::getClip( const std::string & name ) const {
	auto it = this->spriteAnimations.find( name );
	if ( it != this->spriteAnimations.end( ) ) {
		return it->second;
	}
	return nullptr;
}