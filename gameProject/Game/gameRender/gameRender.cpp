#include "gameRender.h"

#include "RenderEntities/RenderEntities.h"
#include "DialogsRender/DialogsRender.h"

#include "../Handlers/entitiesHandler/entitiesHandler.h"
#include "../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../Handlers/gamdDialogHandler/gameDialogHandler.h"
#include "../Managers/gameResourceManager/gameResourceManager.h"

#include "../../Utils/Log/Log.h"
#include "../../Globals/Globals.h"

#include <raylib/raylib.h>
#include <raylib/raymath.h>

Camera2D camera = { 0 };

RenderEntities entititiesRender;
DialogsRender dialogsRender;

bool check = false;

void gameRender::processSoundEvents( ) {

	if ( IsKeyPressed( KEY_SPACE ) ) {
		_gameResourceManager.getMusicManager( )->playMusic( musicType::BossMusic );
	}

	std::string soundEvent = gameSoundsEventHandler::Get( ).getLatestOnQueue( );
	while ( !soundEvent.empty( ) ) {

		if ( !_gameResourceManager.getSoundManager( )->playSound( soundEvent ) ) {
			Log::Print( "[soundEvents] cant play %s" , soundEvent.c_str( ) );
		}

		soundEvent = gameSoundsEventHandler::Get( ).getLatestOnQueue( );
	}
	_gameResourceManager.getMusicManager( )->updateMusic( );
}

void gameRender::renderCustomCursor( ) {
	DrawCircle( globals.mousePosX , globals.mousePosY , 10 , BLUE );
}

void gameRender::renderDialogs( ) {
	dialogsRender.render( );
}

void gameRender::correctMousePosition( ) {

	CPlayerEntity * local = entitiesHandler::Get( ).getLocalPlayer( );
	GVector2D localPos = local->getEntityPosition( );
	static GVector2D oldLocalPos;
	if ( local != nullptr ) {

		GVector2D localPos = local->getEntityPosition( );

		// Define o ponto no mundo a ser seguido
		camera.target = { localPos.x, localPos.y };

		// Centraliza a câmera no meio da tela
		camera.offset = { GetScreenWidth( ) / 2.0f, GetScreenHeight( ) / 2.0f };

		// Converte mouse para coordenadas do mundo
		Vector2 mouseWorld = GetScreenToWorld2D( GetMousePosition( ) , camera );
		GVector2D mousePosWorld( mouseWorld.x , mouseWorld.y );
		globals.mousePosWorldX = mousePosWorld.x;
		globals.mousePosWorldY = mousePosWorld.y;
	}
}


bool DrawButton( Rectangle bounds , const char * label , Color color = LIGHTGRAY , Color textColor = DARKGRAY ) {
	DrawRectangleRec( bounds , color );

	int fontSize = 20;
	int textWidth = MeasureText( label , fontSize );
	int textHeight = fontSize; // Aproximação, funciona bem com fontes padrão

	float textX = bounds.x + ( bounds.width - textWidth ) / 2.0f;
	float textY = bounds.y + ( bounds.height - textHeight ) / 2.0f;

	DrawText( label , textX , textY , fontSize , textColor );

	return CheckCollisionPointRec( GetMousePosition( ) , bounds ) && IsMouseButtonPressed( MOUSE_LEFT_BUTTON );
}


bool DrawSlider( Rectangle bounds , float * value , const char * label , float min , float max , Color backColor , Color frontColor ) {
	bool changed = false;

	// Draw background
	DrawRectangleRec( bounds , backColor );

	// Tamanho do "pino" (círculo)
	float knobRadius = 8.0f;

	// Calcular posição X do knob com base no valor
	float percent = ( *value - min ) / ( max - min );
	float knobX = bounds.x + percent * bounds.width;

	// Interação com mouse
	Vector2 mouse = GetMousePosition( );
	if ( CheckCollisionPointRec( mouse , bounds ) && IsMouseButtonDown( MOUSE_LEFT_BUTTON ) ) {
		percent = ( mouse.x - bounds.x ) / bounds.width;
		percent = Clamp( percent , 0.0f , 1.0f );
		*value = min + percent * ( max - min );
		changed = true;
	}

	// Desenhar barra preenchida
	Rectangle filled = bounds;
	filled.width = percent * bounds.width;
	DrawRectangleRec( filled , frontColor );

	// Desenhar knob (círculo)
	DrawCircle( ( int ) knobX , ( int ) ( bounds.y + bounds.height / 2 ) , knobRadius , RED );

	// Label alinhado à esquerda
	int fontSize = 12;
	DrawText( label , bounds.x , bounds.y - fontSize - 2 , fontSize , WHITE );

	// Valor à direita (com margem de 4px)
	char valueText[ 32 ];
	sprintf_s( valueText , sizeof( valueText ) , "%.0f" , *value );
	int textWidth = MeasureText( valueText , fontSize );
	DrawText( valueText , bounds.x + bounds.width - textWidth , bounds.y - fontSize - 2 , fontSize , WHITE );

	return changed;
}



void gameRender::renderPauseMenu( ) {
	float spacing = 20.0f;
	float contentWidth = 220.0f;

	// Altura dos elementos
	float titleHeight = 40;
	float totalHeight = titleHeight + spacing * 2 + 50 + spacing + 50 + spacing + 20 + spacing + 20;

	Vector2 boxSize = { contentWidth + spacing * 2, totalHeight };
	Vector2 boxCenter = { globals.screenWidth / 2.0f, globals.screenHeight / 2.0f };
	Vector2 boxTopLeft = { boxCenter.x - boxSize.x / 2.0f, boxCenter.y - boxSize.y / 2.0f };

	// Desenha o fundo do menu
	DrawRectangleRec( { boxTopLeft.x, boxTopLeft.y, boxSize.x, boxSize.y } , Fade( DARKGRAY , 0.85f ) );

	// Título
	const char * title = "PAUSADO";
	int titleFontSize = 30;
	int titleWidth = MeasureText( title , titleFontSize );
	DrawText( title , boxCenter.x - titleWidth / 2 , boxTopLeft.y + spacing / 2 , titleFontSize , RAYWHITE );

	// Posição inicial dos componentes
	float y = boxTopLeft.y + titleHeight + spacing;

	Rectangle continueBtn = { boxTopLeft.x + spacing, y, contentWidth, 50 };
	y += continueBtn.height + spacing;

	Rectangle quitBtn = { boxTopLeft.x + spacing, y, contentWidth, 50 };
	y += quitBtn.height + spacing;

	Rectangle musicSlider = { boxTopLeft.x + spacing, y, contentWidth, 20 };
	y += musicSlider.height + spacing;

	Rectangle soundSlider = { boxTopLeft.x + spacing, y, contentWidth, 20 };

	if ( DrawButton( continueBtn , "Continuar" , LIGHTGRAY , DARKGRAY ) ) {
		globals.gamePaused = false;
	}

	if ( DrawButton( quitBtn , "Sair do jogo" , LIGHTGRAY , DARKGRAY ) ) {
		globals.exitGame = true;
	}

	DrawSlider( musicSlider , &globals.MusicVolume , "Volume da música" , 0.0f , 100.0f , GRAY , RED );
	DrawSlider( soundSlider , &globals.SoundVolume , "Volume dos efeitos" , 0.0f , 100.0f , GRAY , RED );
}

void gameRender::render( ) {


	if ( IsKeyPressed( KEY_SPACE ) ) {
		gameDialog newDialog;
		newDialog.dialogText = "Hello World!";
		newDialog.dialogStayTime = 2;
		newDialog.dialogDuration = 10;
		newDialog.triggers.emplace_back( timedTrigger( "localPlayer_hurt" , 2 ) );
		gameDialogHandler::Get( ).throwDialog( newDialog );
	}

	// Zoom com scroll do mouse
	zoomLevel += GetMouseWheelMove( ) * 0.1f;
	zoomLevel = Clamp( zoomLevel , 1.f , 2.0f );
	camera.zoom = zoomLevel;
	correctMousePosition( );




	camera.rotation = 0.0f;

	BeginMode2D( camera );
	{
		entititiesRender.render( );
		
	}
	EndMode2D( );
	renderDialogs( );

	if ( globals.gamePaused ) {
		renderPauseMenu( );
	}
	renderCustomCursor( );
}