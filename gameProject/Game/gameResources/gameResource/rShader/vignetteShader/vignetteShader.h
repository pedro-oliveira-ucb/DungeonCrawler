#pragma once
#include <memory>
#include "../rShader.h"
#include <raylib/raylib.h>

struct vignetteShaderConfig
{
	float radius; // Radius of the vignette effect
	float blur;   // Amount of blur applied to the vignette edge
	Vector3 color;
};

class vignetteShader : public rShader
{
	float vignetteRadius = 0;
	float vignetteBlur = 0;
	Vector3 vignetteColor;

	int resourceRadiusLoc = 0;
	int resourceBlurLoc = 0;
	int resourceColorLoc = 0;

public:
	vignetteShader( rShaderConfig , vignetteShaderConfig );
	vignetteShader( std::shared_ptr<rShader> , vignetteShaderConfig );
	~vignetteShader( );

	void getResourcesLocation( ) override;
	void updateResourcesValues( ) override;
	void renderShader( ) override;
};

