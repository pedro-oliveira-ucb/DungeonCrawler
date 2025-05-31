#include "rMapManager.h"


#include "..\..\..\..\Utils\Log\Log.h"
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

bool rMapManager::initialize( ) {

	std::vector<std::vector<fileScanResult>> FilesOnFolder = this->recursiveGetFiles( this->getPath( ) , ".png" );

	if ( FilesOnFolder.empty( ) ) {
		Log::Print( "[rMapManager] No files found in %s" , this->getPath( ).c_str( ) );
		return false;
	}

	for ( auto & files : FilesOnFolder ) {
		Log::Print( "[rMapManager] Loading!" );

		for ( auto & file : files ) {
			std::shared_ptr<rSprite> sprites;

			sprites = ( std::make_shared<rSprite>( file.filePath ) );

			if ( sprites == nullptr )
				return false;

			std::string mapName = file.recursiveFileName;
		
			this->availableImages.emplace( mapName , std::move( sprites ) );

			Log::Print( "[rMapManager] Loaded %s " , mapName.c_str( ) );
		}
	}

	Log::Print( "[rMapManager] Initialized %d map assets !\nAvailable assets:" , this->availableImages.size( ) );

	for ( const auto & it : this->availableImages ) {
		Log::Print( "[rMapManager] [%s] animation" , it.first.c_str( ) );
	}

	return true;
}

void rMapManager::loadImage( const std::string & name ) {
	fs::path clipPath = name;

	if ( clipPath.empty( ) ) {
		Log::Print( "[rMapManager] Failed to load clip %s, path is empty!" , name.c_str( ) );
		return;
	}

	if ( !fs::exists( clipPath ) || !fs::is_directory( clipPath ) ) {
		Log::Print( "[rMapManager] Folder not found: %s" , clipPath.string( ).c_str( ) );
		return;
	}

	std::vector< std::pair<std::string , std::string >> orderedFiles;

	// Regex para pegar arquivos com nome tipo 0.png, 1.png, ...

	for ( const auto & entry : fs::directory_iterator( clipPath ) ) {
		if ( !entry.is_regular_file( ) ) continue;

		const std::string filename = entry.path( ).filename( ).string( );
		std::smatch match;

		if ( entry.path( ).extension( ) != ".png" ) continue; // Ignora arquivos que não são PNG
		std::string filenameWithoutExt = entry.path( ).stem( ).string( );

		orderedFiles.emplace_back( std::make_pair( entry.path( ).string( ) , filenameWithoutExt ) );
	}


	// Cria os rSprites com os arquivos encontrados
	for ( const auto & [fullPath , filename] : orderedFiles ) {
		Log::Print( "[rSpriteAnimation] Loading sprite: %s" , fullPath.c_str( ) );
		

		fs::path basePath = this->getPath( );
		std::string relativePath = fs::relative( clipPath , basePath ).string( );
		std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
		std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );
		relativePath += ( "_" + filename );

	
		break;
	}
}

std::shared_ptr<rSprite> rMapManager::getMapAsset( const std::string & name ) const {
	auto it = this->availableImages.find( name );
	if ( it != this->availableImages.end( ) ) {
		return it->second;
	}
	return nullptr;
}