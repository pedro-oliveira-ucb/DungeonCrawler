#include "shadersInitializer.h"  

#include "../../gameResources/gameResource/rShader/vignetteShader/vignetteShader.h"  
#include "../../gameResources/gameResource/rShader/fogShader/fogShader.h"  
#include "../../Managers/gameResourceManager/gameResourceManager.h"  
#include "../../Handlers/shadersHandler/shadersHandler.h"


#include "../../../Utils/Log/Log.h"  

bool shadersInitializer::initializeVignetteShaders( ) {  
	std::shared_ptr<rShader> vignetteShaderObject = _gameResourceManager.getShadersManager( )->getShader( "vignette" );  
	if ( vignetteShaderObject == nullptr ) {
		Log::Print( "[shadersInitializer] Failed to get vignette shader on gameResourceManager" );  
		return false;  
	}  

	vignetteShaderConfig config;  
	config.radius = -0.29f;  
	config.blur = 1.1f;  
	config.color = { 0.f, 0.f, 0.f };  

	std::unique_ptr<vignetteShader> vignetteUniqueInstance = std::make_unique<vignetteShader>( vignetteShaderObject, config);
	shaderHandler::Get( ).add( "vignette" , std::move( vignetteUniqueInstance ) );
	return true;  
}  

bool shadersInitializer::initialize( ) {  
	// Implementation for initialize method  
	if(!initializeVignetteShaders() ) {  
		Log::Print( "[shadersInitializer] Failed to initialize vignette shaders" );  
		return false;  
	}

	return true;
}