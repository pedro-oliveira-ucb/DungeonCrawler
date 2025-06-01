#include "fogShader.h"
#include "../../../../Globals/Globals.h"
#include "./../../../../Handlers/shadersHandler/shadersHandler.h"
#include <raylib/raylib.h>

fogShader::fogShader(const rShaderConfig &baseConfig, const FogShaderConfig &cfg)
    : rShader(baseConfig)
{
    fogStart = cfg.fogStart;
    fogEnd = cfg.fogEnd;
    fogColor = cfg.fogColor;
}

fogShader::fogShader( std::shared_ptr<rShader> shader , FogShaderConfig config)
    : rShader(*shader)

{
    fogStart = config.fogStart;
    fogEnd = config.fogEnd;
    fogColor = config.fogColor;
}

void fogShader::getResourcesLocation()
{
    Shader *shader = getShader();
    if (!shader) return;

    locationStart = GetShaderLocation(*shader, "fogStart");
    locationEnd = GetShaderLocation(*shader, "fogEnd");
    locationColor = GetShaderLocation(*shader, "fogColor");
}

void fogShader::updateResourcesValues()
{
    Shader *shader = getShader();
    if (!shader) return;

    SetShaderValue(*shader, locationStart, &fogStart, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*shader, locationEnd, &fogEnd, SHADER_UNIFORM_FLOAT);
    SetShaderValue(*shader, locationColor, &fogColor, SHADER_UNIFORM_VEC3);
}

void fogShader::renderShader()
{
    Shader *shader = getShader();
    if (!shader) return;

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