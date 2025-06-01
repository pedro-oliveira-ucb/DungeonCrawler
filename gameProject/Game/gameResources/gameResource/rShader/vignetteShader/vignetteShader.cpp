#include "vignetteShader.h"

#include "../../../../Handlers/shadersHandler/shadersHandler.h"

#include "../../../../../Utils/Log/Log.h"
#include "../../../../../Globals/Globals.h"

vignetteShader::vignetteShader( rShaderConfig shaderConfig , vignetteShaderConfig vignetteShaderConfig ) :
	rShader( shaderConfig ) {
	this->vignetteBlur = vignetteShaderConfig.blur;
	this->vignetteColor = vignetteShaderConfig.color;
	this->vignetteRadius = vignetteShaderConfig.radius;
}

vignetteShader::vignetteShader( std::shared_ptr<rShader> shader , vignetteShaderConfig vignetteShaderConfig )
	: rShader( *shader ) {
	this->vignetteBlur = vignetteShaderConfig.blur;
	this->vignetteColor = vignetteShaderConfig.color;
	this->vignetteRadius = vignetteShaderConfig.radius;
}

vignetteShader::~vignetteShader( ) {
	
}

void vignetteShader::getResourcesLocation( ) {
	Shader * shader = this->getShader( );

	if ( shader == nullptr )
		return;

	this->resourceRadiusLoc = GetShaderLocation( *shader , "radius" );
	this->resourceBlurLoc = GetShaderLocation( *shader , "blur" );
	this->resourceColorLoc = GetShaderLocation( *shader , "color" );
}

void vignetteShader::updateResourcesValues( ) {
	Shader * shader = this->getShader( );

	if ( shader == nullptr )
		return;

	SetShaderValue( *shader , this->resourceRadiusLoc , &this->vignetteRadius , SHADER_UNIFORM_FLOAT );
	SetShaderValue( *shader , this->resourceBlurLoc , &this->vignetteBlur , SHADER_UNIFORM_FLOAT );
	SetShaderValue( *shader , this->resourceColorLoc , &this->vignetteColor , SHADER_UNIFORM_VEC3 );
}

void vignetteShader::renderShader( ) {
	Shader * shader = this->getShader( );

	void * shaderTexture = shaderHandler::Get( ).getRenderTexture( );

	RenderTexture2D * shaderTexture2D = static_cast< RenderTexture2D * >( shaderTexture );

	if ( shader == nullptr || shaderTexture2D == nullptr )
		return;

	// Draw vignette.
	BeginShaderMode( *shader );
	Rectangle sourceRec = { 0.0f, 0.0f, ( float ) shaderTexture2D->texture.width, ( float ) shaderTexture2D->texture.height };
	Vector2 destRec = { 0.0f, 0.0f };
	DrawTextureRec(
		shaderTexture2D->texture ,
		sourceRec ,
		destRec ,
		BLANK
	);
	EndShaderMode( );
}