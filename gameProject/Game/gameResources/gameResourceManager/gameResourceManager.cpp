#include "gameResourceManager.h"

#include "../../../Utils/Log/Log.h"

bool gameResourceManager::initialize( std::string path ) {
    this->basePath = path;
    this->spritesMananger = std::make_unique<rSpritesManager>( path + "\\sprites" );

    if ( !this->spritesMananger->initialize( ) ) {
        Log::Print( "[gameResourceManager] rSpritesManager initialization failed!" );
        return false;
    }

    this->soundsMananger = std::make_unique<rSoundsManager>( path + "\\sounds" );

    if ( !this->soundsMananger->initialize( ) ) {
        Log::Print( "[gameResourceManager] rSoundsManager initialization failed!" );
        return false;
    }

    return true;
}

rSpritesManager & gameResourceManager::getSpritesManager( ) {
    return *this->spritesMananger;
}
