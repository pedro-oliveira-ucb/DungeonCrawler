#include "shadersHandler.h"

#include "../../../Globals/Globals.h"
#include <raylib/raylib.h>

void shaderHandler::initializeRenderTexture( )
{
    RenderTexture2D * txt = new RenderTexture2D( );
	*txt = LoadRenderTexture( Globals::Get().screenWidth , Globals::Get( ).screenHeight );
    this->renderTexture = static_cast< void * >( txt );
}

void shaderHandler::add( const std::string & name , std::unique_ptr<rShader> shader ) {
    std::lock_guard<std::mutex> lock( mutex );
    shaders[ name ] = std::move( shader );
}

rShader * shaderHandler::get( const std::string & name ) {
    std::lock_guard<std::mutex> lock( mutex );
    auto it = shaders.find( name );
    if ( it != shaders.end( ) ) return it->second.get( );
    return nullptr;
}

void shaderHandler::preLoadAll( ){
	std::lock_guard<std::mutex> lock( mutex );  
    for( auto & shaderPair : shaders ) {
        if ( shaderPair.second ) {
            shaderPair.second->getResourcesLocation( );
        }
	}
}

void shaderHandler::updateAll( ){
    std::lock_guard<std::mutex> lock( mutex );
    for ( auto & shaderPair : shaders ) {
        if ( shaderPair.second ) {
            shaderPair.second->updateResourcesValues( );
        }
    }
}

void shaderHandler::renderAll( ){
    std::lock_guard<std::mutex> lock( mutex );
    for ( auto & shaderPair : shaders ) {
        if ( shaderPair.second ) {
            shaderPair.second->renderShader( );
        }
    }
}
