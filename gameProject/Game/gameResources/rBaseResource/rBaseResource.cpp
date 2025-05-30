#include "rBaseResource.h"


#include "..\..\..\..\Utils\Log\Log.h"
#include <filesystem>

namespace fs = std::filesystem;

rBaseResource::rBaseResource( std::string path ) {
	this->Path = path;
}

std::string rBaseResource::getPath( ) {
	return this->Path;
}

std::vector<std::vector<fileScanResult>> rBaseResource::recursiveGetFiles( std::string path , std::string extension )
{
	fs::path basePath = this->getPath( );
	std::vector<std::vector<fileScanResult>> files;

	if ( !fs::exists( basePath ) ) {
		Log::Print( "[rBaseResource] %s doesn't exist!" , basePath.string( ).c_str( ) );
		return files;
	}

	Log::Print( "[rBaseResource] Recursively scanning folders in: %s" , basePath.string( ).c_str( ) );

	std::vector<std::string> toLoad;

	for ( const auto & entry : fs::recursive_directory_iterator( basePath ) ) {
		if ( !fs::is_directory( entry ) ) continue;

		bool containsDesiredFile = false;

		// verifica se a pasta tem arquivos tipo 0.png, 1.png, etc.
		for ( const auto & file : fs::directory_iterator( entry ) ) {
			if ( fs::is_regular_file( file ) ) {
				std::string filename = file.path( ).filename( ).string( );
				if ( file.path( ).extension( ) != extension )
					continue;

				containsDesiredFile = true;
				break;
			}
		}

		if ( containsDesiredFile ) {
			Log::Print( "[rBaseResource] Found %s on folder: %s" , extension.c_str( ) , entry.path( ).string( ).c_str( ) );
			toLoad.emplace_back( entry.path( ).string( ).c_str( ) );
		}
	}

	for ( auto & folder : toLoad ) {
		std::vector<fileScanResult> result = this->getRecursiveFiles( folder , extension );
		if ( !result.empty( ) ) {
			files.emplace_back( result );
		}
	}

	return files;
}


std::vector<fileScanResult> rBaseResource::getRecursiveFiles( std::string path , std::string extension ) {
	fs::path clipPath = path;

	std::vector<fileScanResult> resultFiles;

	if ( clipPath.empty( ) ) {
		Log::Print( "[rBaseResource] Failed to load file %s, path is empty!" , path.c_str( ) );
		return resultFiles;
	}

	if ( !fs::exists( clipPath ) || !fs::is_directory( clipPath ) ) {
		Log::Print( "[rBaseResource] Folder not found: %s" , clipPath.string( ).c_str( ) );
		return resultFiles;
	}

	fs::path basePath = this->getPath( );
	std::string relativePath = fs::relative( clipPath , basePath ).string( );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );

	for ( const auto & entry : fs::directory_iterator( clipPath ) ) {
		if ( !entry.is_regular_file( ) ) continue;

		if ( entry.path( ).extension( ) != extension ) continue; // Ignora arquivos que não são PNG
		const std::string filenameWithoutExt = entry.path( ).stem( ).string( );
		const std::string filename = entry.path( ).filename( ).string( );
		const std::string fullpath = entry.path( ).string( );

		fileScanResult file;
		file.filePath = fullpath;
		file.fileName = filenameWithoutExt;
		file.rawFileName = filename;
		file.recursiveFileName = relativePath + "_" + filenameWithoutExt;
		file.rawRecursivePath = relativePath;
		file.folderPath = path;

		resultFiles.emplace_back( file );
	}

	return resultFiles;
}

