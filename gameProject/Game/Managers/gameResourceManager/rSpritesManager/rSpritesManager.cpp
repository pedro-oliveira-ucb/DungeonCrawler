#include "rSpritesManager.h"
#include "..\..\..\..\Utils\Log\Log.h"
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

bool rSpritesManager::initialize( ) {
	std::vector<std::vector<fileScanResult>> FilesOnFolder = this->recursiveGetFiles( this->getPath( ) , ".png" );

	if ( FilesOnFolder.empty( ) ) {
		Log::Print( "[rMapManager] No files found in %s" , this->getPath( ).c_str( ) );
		return false;
	}

	// Regex para pegar arquivos com nome tipo 0.png, 1.png, ...
	std::regex pattern( R"((\d+)\.png)" );

	for ( auto & files : FilesOnFolder ) {
		std::vector<std::pair<int , fileScanResult>> orderedFiles; // Para ordenar os arquivos por nome (0.png, 1.png, ...)
		std::string relativePath;

		bool found = false;

		for ( auto & file : files ) {
			std::smatch match;
			if ( std::regex_match( file.rawFileName , match , pattern ) ) {
				int index = std::stoi( match[ 1 ].str( ) );
				orderedFiles.emplace_back( index , file );
				relativePath = file.rawRecursivePath;
				found = true;
			}
		}

		if ( !found ) {
			continue;
		}

		std::vector<std::shared_ptr<rSprite>> sprites;
		// Cria os rSprites com os arquivos encontrados
		for ( const auto & [index , file] : orderedFiles ) {
			Log::Print( "[rSpriteAnimation] Loading sprite: %s" , file.filePath.c_str( ) );
			sprites.push_back( std::make_shared<rSprite>( file.filePath ) );
		}

		Log::Print( "[rSpritesManager] Loaded %d sprites" , static_cast< int >( sprites.size( ) ) );

		availableSprites.emplace( files.at( 0 ).recursiveFileName , std::move( sprites ) );

		rSpriteAnimation * animation = new rSpriteAnimation( availableSprites.at( files.at( 0 ).recursiveFileName ) , 24 );
		std::shared_ptr< rSpriteAnimation> sharedAnimationPtr( animation );

		this->spriteAnimations.emplace( relativePath.c_str( ) , sharedAnimationPtr );
	}

	Log::Print( "[rSpritesManager] Initialized %d sprites !\nAvailable sprites:" , this->spriteAnimations.size( ) );

	for ( const auto & it : this->spriteAnimations ) {
		Log::Print( "[rSpritesManager] [%s] animation" , it.first.c_str( ) );
	}

	return true;
}

std::vector<std::shared_ptr<rSprite>> * rSpritesManager::getSprite( const std::string & name ) {
	if ( this->availableSprites.find( name ) != this->availableSprites.end( ) ) {
		return &this->availableSprites.at( name );
	}

	return nullptr;
}

std::shared_ptr<rSpriteAnimation> rSpritesManager::getClip( const std::string & name ) const {
	auto it = this->spriteAnimations.find( name );
	if ( it != this->spriteAnimations.end( ) ) {
		return it->second;
	}
	return nullptr;
}