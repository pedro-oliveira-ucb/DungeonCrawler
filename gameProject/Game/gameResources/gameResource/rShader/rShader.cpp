#include "rShader.h"
#include <raylib/raylib.h>
#include "../../../../Utils/Log/Log.h"

rShader::rShader( const rShader & other )
{
	Log::Print( "[rShader] Copy constructor called" );
	this->config = other.config;
	this->shader = other.shader;
	this->initialized = other.initialized;
}

rShader::rShader( const rShaderConfig & cfg )
	: config( cfg )
{
	Log::Print( "[rShader] Constructor called with config: %s, %s" , cfg.vsPath.c_str( ) , cfg.fsPath.c_str( ) );
	this->shader = nullptr;
	this->initialized = false;
	if ( !this->initialize( ) ) {
		Log::Print( "[rShader] Failed to initialize shader with config: %s, %s" , cfg.vsPath.c_str( ) , cfg.fsPath.c_str( ) );
	}
}

bool rShader::initialize( )
{
	if ( initialized ) return true;

	this->shader = std::make_shared<Shader>( );


	*shader = LoadShader( config.vsPath.c_str( ) , config.fsPath.c_str() );

	if ( shader->id <= 0 ) {
		Log::Print( "[rShader] Falha ao carregar shader: %s e %s" , config.vsPath.c_str( ) , config.fsPath.c_str( ) );
		return false;
	}

	initialized = true;
	return true;
}


void * rShader::getRenderTexture( ) {
	return nullptr;
}
void rShader::getResourcesLocation( ) {

}
void rShader::updateResourcesValues( ) {

}
void rShader::renderShader( ) {

}