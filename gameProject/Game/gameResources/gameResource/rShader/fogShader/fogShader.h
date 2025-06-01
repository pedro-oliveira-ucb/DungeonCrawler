#pragma once
#include "../rShader.h"
#include <raylib/raylib.h>

struct FogShaderConfig
{
    float fogStart;
    float fogEnd;
    Vector3 fogColor;
};

class fogShader : public rShader
{
private:
    float fogStart;
    float fogEnd;
    Vector3 fogColor;

    int locationStart;
    int locationEnd;
    int locationColor;

public:
    fogShader(const rShaderConfig &baseConfig, const FogShaderConfig &cfg);
    fogShader( std::shared_ptr<rShader> , FogShaderConfig );
    void getResourcesLocation() override;
    void updateResourcesValues() override;
    void renderShader() override;
};