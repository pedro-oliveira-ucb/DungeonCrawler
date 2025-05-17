#include "rSpriteAnimation.h"
#include "../../../Utils/Log/Log.h"

#include <filesystem>
#include <regex>
#include <algorithm>

namespace fs = std::filesystem;

void rSpriteAnimation::initializeAnimation( ) {
    Log::Print( "[rSpriteAnimation] Initializing %s" , this->getModelName( ) );

    // this->modelFolderPath = this->modelName; // Define o path base, pode ser ajustado se necessário

    if ( !fs::exists( modelFolderPath ) || !fs::is_directory( modelFolderPath ) ) {
        Log::Print( "[rSpriteAnimation] Folder not found: %s" , modelFolderPath.c_str( ) );
        return;
    }

    std::vector<std::pair<int , std::string>> orderedFiles;

    // Regex para pegar arquivos com nome tipo 0.png, 1.png, ...
    std::regex pattern( R"((\d+)\.png)" );

    for ( const auto & entry : fs::directory_iterator( modelFolderPath ) ) {
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

    // Cria os rSprites com os arquivos encontrados
    for ( const auto & [index , fullPath] : orderedFiles ) {
        Log::Print( "[rSpriteAnimation] Loading sprite: %s" , fullPath.c_str( ) );
        sprites.push_back( std::make_unique<rSprite>( fullPath ) );
    }

    Log::Print( "[rSpriteAnimation] Loaded %d sprites" , static_cast< int >( this->sprites.size( ) ) );
}

std::string rSpriteAnimation::getModelName( ) {
	return this->modelName;
}

std::string rSpriteAnimation::getModelPath( ) {
	return this->modelFolderPath;
}

void rSpriteAnimation::updateAnimation( bool loop ) {
    if ( ( this->currentAnimationStep + 1 ) >= sprites.size( )) {
        this->currentAnimationStep = 0;
    }
    else
        this->currentAnimationStep++;
}

Texture2D * rSpriteAnimation::getCurrentTexture( ) {
    if ( sprites.empty( ) ) return nullptr;
    return sprites[ currentAnimationStep ]->getTexture( );
}