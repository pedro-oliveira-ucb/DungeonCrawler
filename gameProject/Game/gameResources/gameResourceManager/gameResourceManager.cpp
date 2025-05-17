#include "gameResourceManager.h"

#include "../../../Utils/Log/Log.h"

bool gameResourceManager::initialize( std::string path ) {
    this->basePath = path;
    this->spritesMananger = std::make_unique<rSpritesManager>( path + "\\sprites" );

    if ( !this->spritesMananger->initialize( ) ) {
        Log::Print( "[gameResourceManager] gameResourceManager initialization failed!" );
        return false;
    }

    return true;
}

rSpritesManager & gameResourceManager::getSpritesManager( ) {
    return *this->spritesMananger;
}
