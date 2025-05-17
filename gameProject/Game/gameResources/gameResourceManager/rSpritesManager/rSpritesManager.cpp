#include "rSpritesManager.h"
#include "../../../Utils/Log/Log.h"
#include <filesystem>

namespace fs = std::filesystem;

bool rSpritesManager::initialize( ) {

    fs::path basePath = this->getPath( );

    if ( !fs::exists( basePath ) ) {
        Log::Print( "[rSpritesManager] %s doesn't exist!" , basePath.string( ).c_str( ) );
        return false;
    }

    Log::Print( "[rSpritesManager] Listing folders in: %s" , basePath.string( ).c_str( ) );

    for ( const auto & entry : fs::directory_iterator( basePath ) ) {
        if ( fs::is_directory( entry ) ) {
            Log::Print( "[rSpritesManager] Found folder: %s" , entry.path( ).filename( ).string( ).c_str( ) );
            this->spriteAnimations.emplace(
                entry.path( ).filename( ).string( ) ,
                rSpriteAnimation( entry.path( ).filename( ).string( ) , entry.path( ).string( ) )
            );
        }
    }

    Log::Print( "[rSpritesManager] Initialized %d animations!\nAvailable animations:" , this->spriteAnimations.size() );

    for ( auto it = this->spriteAnimations.begin( ); it != this->spriteAnimations.end( ); ++it ) {
        Log::Print( "[rSpritesManager] [%s] animation" , it->first );
    }

    return true;
}

std::unordered_map<std::string , rSpriteAnimation> * rSpritesManager::getSpriteAnimations( ) {
    return &this->spriteAnimations;
}