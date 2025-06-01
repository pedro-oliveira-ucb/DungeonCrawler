#pragma once
#include <string>
#include <memory>

struct Shader; // Forward declaration

struct rShaderConfig
{
	std::string vsPath;  // Caminho do vertex shader
	std::string fsPath;  // Caminho do fragment shader
};

class rShader
{
protected:
	Shader * getShader( ) { return shader.get(); }
	rShader( const rShader & );
private:
	std::shared_ptr<Shader> shader;
	rShaderConfig config;
	bool initialized = false;

public:
	rShader( const rShaderConfig & cfg );

	bool initialize( );
	virtual void getResourcesLocation( );
	virtual void updateResourcesValues( );
	virtual void renderShader( );
	virtual void * getRenderTexture( );
};
