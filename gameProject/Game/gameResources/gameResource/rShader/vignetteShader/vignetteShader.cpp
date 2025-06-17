#include "vignetteShader.h"

#include "../../../../Handlers/shadersHandler/shadersHandler.h"

#include "../../../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../../../../Utils/Log/Log.h"
#include "../../../../../Globals/Globals.h"

#include <raylib/raymath.h>

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

	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );

	if ( player == nullptr ) {
		Log::Print( "vignetteShader::updateResourcesValues: Player entity is null." );
	}
	else {
		const float playerHealth = player->getHealth( );
		const float playerMaxHealth = player->getMaxHealth( );
		float healthPercentage = playerHealth / playerMaxHealth;

		// If player health is 0, set radius to a very low value to avoid division by zero.
		if ( playerHealth <= 0 ) {
			this->vignetteRadius = -0.65f; // Set radius to a very low value to avoid division by zero.
		}
		else {
			// Calculate the vignette color based on player health.
			// The color will be more intense as the player health decreases.
			// The color will be a shade of red, with the intensity based on the health percentage.
			Vector3 currentShaderColor = this->vignetteColor;


			if ( healthPercentage < 0.3 ) {
				currentShaderColor.x = 1.0f - std::max( ( playerHealth / playerMaxHealth ) , 0.0f ); // Red channel
				currentShaderColor.x = Clamp( currentShaderColor.x , 0.0f , 0.2f );
			}
			else {
				currentShaderColor.x = 0.0f;
			}

			currentShaderColor.y = 0.0f; // Green channel
			currentShaderColor.z = 0.0f; // Blue channel
			this->vignetteColor = currentShaderColor;

			// Calculate the vignette radius based on player health.
			float currentShaderRadius = this->vignetteRadius;
			currentShaderRadius = ( -0.29f * playerMaxHealth ) / playerHealth;
			this->vignetteRadius = std::max( currentShaderRadius , -0.65f );
		}
	}

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