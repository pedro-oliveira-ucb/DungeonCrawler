#pragma once
#include <string>

struct Shader; // Forward declaration

struct rShaderConfig
{
    std::string vsPath;  // Caminho do vertex shader
    std::string fsPath;  // Caminho do fragment shader
};

class rShader
{
private:
    Shader * shader;        
    rShaderConfig config;
    bool initialized = false;

public:
    rShader( const rShaderConfig & cfg );

    bool initialize( );
    void beginShaderMode( );
    void endShaderMode( );
};
