#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include "../../../Utils/singleton.h"
#include "../../gameResources/gameResource/rShader/rShader.h"

class shaderHandler : public CSingleton< shaderHandler > {
private:
	std::mutex mutex;
	std::unordered_map<std::string , std::unique_ptr<rShader>> shaders;

	void * renderTexture = nullptr;

public:
	void initializeRenderTexture( );
	void * getRenderTexture( ) const { return renderTexture; }

	void add( const std::string & name , std::unique_ptr<rShader> shader );

	rShader * get( const std::string & name );

	void preLoadAll( );
	void updateAll( );
	void renderAll( );
};
