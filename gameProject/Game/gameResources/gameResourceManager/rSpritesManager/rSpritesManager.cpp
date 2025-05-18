#include "rSpritesManager.h"
#include "../../../Utils/Log/Log.h"
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
            std::string relativePath = fs::relative( entry.path( ) , basePath ).string( );
            std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
            std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );

            Log::Print( "[rSpritesManager] Found sprite animation folder: %s" , relativePath.c_str( ) );

            this->spriteAnimations.emplace(
                relativePath ,
                rSpriteAnimation( relativePath , entry.path( ).string( ) )
            );
        }
    }

    Log::Print( "[rSpritesManager] Initialized %d animations!\nAvailable animations:" , this->spriteAnimations.size( ) );

    for ( const auto & it : this->spriteAnimations ) {
        Log::Print( "[rSpritesManager] [%s] animation" , it.first.c_str( ) );
    }

    return true;
}

std::unordered_map<std::string , rSpriteAnimation> * rSpritesManager::getSpriteAnimations( ) {
    return &this->spriteAnimations;
}
