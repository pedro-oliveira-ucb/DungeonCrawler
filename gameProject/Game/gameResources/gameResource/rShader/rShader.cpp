#include "rShader.h"
#include <raylib/raylib.h>
#include "../../../../Utils/Log/Log.h"

rShader::rShader( const rShaderConfig & cfg )
	: config( cfg )
{
}

bool rShader::initialize( )
{
	if ( initialized ) return true;

	shader = new Shader( );

	*shader = LoadShader( config.vsPath.c_str( ) , config.fsPath.c_str( ) );
	if ( shader->id <= 0 )
	{
		Log::Print( "[rShader] Falha ao carregar shader: %s e %s" , config.vsPath.c_str( ) , config.fsPath.c_str( ) );
		delete shader;
		return false;
	}

	initialized = true;
	return true;
}

void rShader::beginShaderMode( )
{
	if ( initialized )
		BeginShaderMode( *shader );
}

void rShader::endShaderMode( )
{
	if ( initialized )
		EndShaderMode( );
}
