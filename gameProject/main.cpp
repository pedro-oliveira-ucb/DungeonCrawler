#include <iostream>
#include <thread>
#include <sstream>
#include <iomanip>
#include <memory>

#include "Game/game.h"
#include "Game/gameRender/gameRender.h"
#include "Game/gameControls/keybindHandler/keybindHandler.h"
#include "Game/gameObjects/gameState/inPreMainMenuState/inPreMainMenuState.h"
#include "Game/Managers/gameResourceManager/gameResourceManager.h"
#include "Game/Managers/gameStateManagers/gameStateManager.h"
#include "Game/Handlers/shadersHandler/shadersHandler.h"
#include "Globals/Globals.h"
#include "Utils/Log/Log.h"
#include "Utils/utils.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>

// Para verificação de vazamento de memória em modo Debug
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>


void Initialize( );
void GameLoop( );
void Update( gameStateManager & gameStateManager );
void Render( gameStateManager & gameStateManager );
void Shutdown( );
void ClampMouseToBounds( );

// Objeto principal do jogo.
Game game;

/// @brief Ponto de entrada principal do programa.
int main( void ) {
#if _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	Initialize( );

	if ( game.isResourcesLoaded( ) ) {
		GameLoop( );
	}
	else {
		Log::Print( "[main] Falha ao carregar recursos do jogo. Encerrando." );
	}

	Shutdown( );

	return 0;
}

/// @brief Realiza toda a configuração inicial do jogo, janela e recursos.
void Initialize( ) {
	Log::Print( "[main] Iniciando a inicializacao..." );

	std::pair<int , int> screenSize = utils::Get( ).getScreenSize( );
	Globals::Get( ).screenWidth = screenSize.first;
	Globals::Get( ).screenHeight = screenSize.second;
	Log::Print( "[Globals] Tamanho da tela: %dx%d" , screenSize.first , screenSize.second );

	SetConfigFlags( FLAG_VSYNC_HINT );
	// SetConfigFlags(FLAG_FULLSCREEN_MODE); // Descomente para modo tela cheia.
	InitWindow( Globals::Get( ).screenWidth , Globals::Get( ).screenHeight , "Janela do Jogo" );
	InitAudioDevice( );
	DisableCursor( ); // Desabilita o cursor do sistema para usar um cursor customizado.

	BeginDrawing( );
	ClearBackground( BLACK );
	DrawText( "Loading..." , GetScreenWidth( ) / 2 - MeasureText( "Loading..." , 20 ) / 2 , GetScreenHeight( ) / 2 - 10 , 20 , WHITE );
	EndDrawing( );

	auto defaultFont = std::make_unique<Font>( LoadFontEx( "font.ttf" , 32 , 0 , 0 ) );
	Globals::Get( ).setDefaultFont( defaultFont.get( ) );

	Log::Print( "[main] Iniciando o jogo..." );
	if ( !game.start( ) ) {
		Log::Print( "[main] Falha ao iniciar o jogo!" );
		return; // Retorna para evitar continuar com o jogo em um estado inválido.
	}

	// Poderia ser uma tela de carregamento assíncrona.
	while ( !game.isResourcesLoaded( ) ) {
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}

	Log::Print( "[main] Recursos carregados. Inicializando shaders..." );
	shaderHandler::Get( ).initializeRenderTexture( );
	shaderHandler::Get( ).preLoadAll( );
}

/// @brief Contém o loop principal do jogo, onde a lógica é atualizada e renderizada.
void GameLoop( ) {
	gameStateManager gameStateManager_;
	gameStateManager_.ChangeState( std::make_unique<inPreMainMenuState>( ) );

	Log::Print( "[main] Entrando no loop do jogo." );

	// O loop principal continua enquanto o usuário não apertar delete
	while ( !IsKeyPressed( KEY_DELETE ) ) {
		Update( gameStateManager_ );
		Render( gameStateManager_ );
	}
}

/// @brief Atualiza a lógica do jogo a cada frame.
/// @param gameStateManager O gerenciador de estados do jogo.
void Update( gameStateManager & gameStateManager_ ) {
	float deltaTime = GetFrameTime( );
	Globals::Get( ).setFrameTime( deltaTime );

	// Mantém o cursor do mouse dentro dos limites da janela.
	ClampMouseToBounds( );

	_gameResourceManager.getSoundManager( )->setVolume( Globals::Get( ).getGameSettings( )->getSoundVolume( ) );
	_gameResourceManager.getMusicManager( )->setMusicVolume( Globals::Get( ).getGameSettings( )->getMusicVolume( ) );

	gameStateManager_.HandleInput( );
	gameStateManager_.Update( deltaTime );

	keybindHandler::Get( ).update( );
	gameRender::Get( ).processSoundEvents( );
	shaderHandler::Get( ).updateAll( );
}

/// @brief Desenha todos os elementos visuais na tela.
/// @param gameStateManager O gerenciador de estados do jogo.
void Render( gameStateManager & gameStateManager_ ) {
	BeginDrawing( );
	ClearBackground( WHITE ); // Limpa a tela.

	gameStateManager_.Render( );

	gameRender::Get( ).renderCustomCursor( );

	EndDrawing( );
}

/// @brief Limpa os recursos e fecha o aplicativo.
void Shutdown( ) {
	Log::Print( "[main] Encerrando o jogo e liberando recursos." );

	CloseAudioDevice( );
	CloseWindow( );

#if _DEBUG
	system( "pause" ); // Pausa o console no final em modo de depuração.
#endif
}

/// @brief Garante que o cursor do mouse permaneça dentro dos limites da janela.
void ClampMouseToBounds( ) {
	Vector2 mousePos = GetMousePosition( );

	// Usa a função Clamp de raymath.h para limitar os valores.
	mousePos.x = Clamp( mousePos.x , 0.0f , ( float ) GetScreenWidth( ) );
	mousePos.y = Clamp( mousePos.y , 0.0f , ( float ) GetScreenHeight( ) );

	SetMousePosition( mousePos.x , mousePos.y );

	Globals::Get( ).mousePosX = mousePos.x;
	Globals::Get( ).mousePosY = mousePos.y;
}
