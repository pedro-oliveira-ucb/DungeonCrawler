#include "renderPauseMenu.h"

#include <raylib/raylib.h>

#include "../../../Globals/Globals.h"
#include "../components/components.h"

void renderPauseMenu::render( ) {
	float spacing = 20.0f;
	float contentWidth = 220.0f;

	// Altura dos elementos
	float titleHeight = 40;
	float totalHeight = titleHeight + spacing * 2 + 50 + spacing + 50 + spacing + 20 + spacing + 20;

	Vector2 boxSize = { contentWidth + spacing * 2, totalHeight };
	Vector2 boxCenter = { Globals::Get().screenWidth / 2.0f, Globals::Get( ).screenHeight / 2.0f };
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

	if ( components::Get().DrawButton( continueBtn , "Continuar" , LIGHTGRAY , DARKGRAY ) ) {
		Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_PLAYING );
	}

	if ( components::Get( ).DrawButton( quitBtn , "Sair do jogo" , LIGHTGRAY , DARKGRAY ) ) {
		Globals::Get( ).getGame( )->setCurrentGameState( currentGameState::GAME_STATE_EXIT );
	}

	float musicVolume = Globals::Get( ).getGameSettings( )->getMusicVolume( );
	float soundVolume = Globals::Get( ).getGameSettings( )->getSoundVolume( );

	components::Get( ).DrawSlider( musicSlider , &musicVolume , "Volume da música" , 0.0f , 100.0f , GRAY , RED );
	components::Get( ).DrawSlider( soundSlider , &soundVolume , "Volume dos efeitos" , 0.0f , 100.0f , GRAY , RED );

	Globals::Get( ).getGameSettings( )->setMusicVolume( musicVolume );
	Globals::Get( ).getGameSettings( )->setSoundVolume( soundVolume );
}