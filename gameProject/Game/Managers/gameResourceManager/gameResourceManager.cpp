#include "gameResourceManager.h"

#include "../../../Utils/Log/Log.h"

bool gameResourceManager::initialize( std::string path ) {
    this->basePath = path;
    this->mapManagerManager = std::make_unique<rMapManager>( path + "\\map" );

    if ( !this->mapManagerManager->initialize( ) ) {
        Log::Print( "[gameResourceManager] rMapManager initialization failed!" );
        return false;
    }

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
    this->musicMananger = std::make_unique<rMusicManager>( path + "\\musics" );

    if ( !this->musicMananger->initialize( ) ) {
        Log::Print( "[gameResourceManager] rMusicManager initialization failed!" );
        return false;
    }

    this->shadersManager = std::make_unique<rShadersManager>( path + "\\shaders" );

    if ( !this->shadersManager->initialize( ) ) {
        Log::Print( "[gameResourceManager] rShadersManager initialization failed!" );
        return false;
    }

  

    this->musicMananger->playMusic( musicType::DungeonMusic );
    return true;
}

rMusicManager * gameResourceManager::getMusicManager( ) {
	return this->musicMananger.get( );
}

rShadersManager * gameResourceManager::getShadersManager( ) {
    return this->shadersManager.get( );
}

rMapManager * gameResourceManager::getMapManager( ){
    return this->mapManagerManager.get( );
}

rSpritesManager *gameResourceManager::getSpritesManager( ) {
    return this->spritesMananger.get();
}

rSoundsManager *gameResourceManager::getSoundManager( ) {
    return this->soundsMananger.get();
}

