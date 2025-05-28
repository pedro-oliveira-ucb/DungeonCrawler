#include "rShadersManager.h"
#include <filesystem>
#include <raylib/raylib.h>
#include "../../../Utils/Log/Log.h"

namespace fs = std::filesystem;

bool rShadersManager::initialize( )
{
	fs::path basePath = this->getPath( );

	if ( !fs::exists( basePath ) )
	{
		Log::Print( "[rShadersManager] Caminho inexistente: %s" , basePath.string( ).c_str( ) );
		return false;
	}

	Log::Print( "[rShadersManager] Recursively scanning folders in: %s" , basePath.string( ).c_str( ) );

	std::vector<std::string> toLoad;

	for ( const auto & entry : fs::recursive_directory_iterator( basePath ) ) {
		if ( !fs::is_directory( entry ) ) continue;

		bool containsshader = false;

		// verifica se a pasta tem arquivos tipo 0.png, 1.png, etc.
		for ( const auto & file : fs::directory_iterator( entry ) ) {
			if ( fs::is_regular_file( file ) ) {
				std::string filename = file.path( ).filename( ).string( );
				containsshader = true;
				break;
			}
		}

		if ( containsshader ) {
			// gera chave tipo localplayer_idle_backward
			Log::Print( "[rShadersManager] Found shader folder: %s" , entry.path( ).string( ).c_str( ) );
			toLoad.emplace_back( entry.path( ).string( ).c_str( ) );
		}
	}

	for ( auto animationPath : toLoad ) {
		Log::Print( "[rShadersManager] Loading %s!" , animationPath.c_str( ) );
		if ( !this->loadShader( animationPath ) ) {
			Log::Print( "[rShadersManager] Failed to load shader %s!" , animationPath.c_str( ) );
			return false;
		}
		else {
			Log::Print( "[rShadersManager] Successfully loaded shader %s!" , animationPath.c_str( ) );
		}
	}

	Log::Print( "[rShadersManager] Initialized %d shader!\nAvailable shader:" , this->shaders.size( ) );

	for ( const auto & it : this->shaders ) {
		Log::Print( "[rShadersManager] [%s] shader" , it.first.c_str( ) );
	}

	return true;
}

bool rShadersManager::loadShader( const std::string & name )
{
	fs::path clipPath = name;
	fs::path configPath = name + "\\config.json";

	if ( clipPath.empty( ) ) {
		Log::Print( "[rShadersManager] Failed to load clip %s, path is empty!" , name.c_str( ) );
		return false;
	}

	if ( !fs::exists( clipPath ) || !fs::is_directory( clipPath ) ) {
		Log::Print( "[rShadersManager] Folder not found: %s" , clipPath.string( ).c_str( ) );
		return false;
	}

	bool fs = false;

	std::string shaderFilePath;
	for ( const auto & entry : fs::directory_iterator( clipPath ) ) {
		if ( !entry.is_regular_file( ) ) continue;

		if ( entry.path( ).extension( ) != ".fs" ) {
			fs = false;
			if(entry.path().extension() != ".vs") {
				continue;
			}
		}
		// only load .fs and .vs files
		const std::string filename = entry.path( ).string( );
		shaderFilePath = filename;
		break;
	}

	if ( shaderFilePath.empty( ) ) {
		Log::Print( "[rShadersManager] No shader file found in %s!" , clipPath.string( ).c_str( ) );
		return false;
	}

	fs::path basePath = this->getPath( );
	std::string relativePath = fs::relative( clipPath , basePath ).string( );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '/' , '_' );
	std::replace( relativePath.begin( ) , relativePath.end( ) , '\\' , '_' );

	// Exemplo simples de load com caminhos fixos
	rShaderConfig cfg { fs ? "" : shaderFilePath, fs ? shaderFilePath : "" };
	auto shaderPtr = std::make_unique<rShader>( cfg );
	if ( !shaderPtr->initialize( ) ){
		Log::Print( "[rShadersManager] Failed to initialize shader %s!" , name.c_str( ) );
		return false;
	}

	shaders.emplace( relativePath , std::move( shaderPtr ) );
	return true;
}

bool rShadersManager::addShader( const std::string & key , const rShaderConfig & config )
{
	auto shaderPtr = std::make_unique<rShader>( config );
	if ( !shaderPtr->initialize( ) )
		return false;

	std::lock_guard<std::mutex> lock( mtx );
	shaders.emplace( key , std::move( shaderPtr ) );
	return true;
}

void rShadersManager::renderAllShaders( )
{
	// Exemplo para demonstrar encapsulação, caso queira aplicar cada shader
	std::lock_guard<std::mutex> lock( mtx );
	for ( auto & pair : shaders )
	{
		 pair.second->beginShaderMode();
			
		 pair.second->endShaderMode();
	}
}