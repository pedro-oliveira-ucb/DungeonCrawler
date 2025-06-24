#include "components.h"

#include <raylib/raymath.h>
#include <stdio.h>


bool components::DrawButton( Rectangle bounds , const char * label , Color color , Color textColor, Color borderColor) {


	int fontSize = 20;
	int textWidth = MeasureText( label , fontSize );
	int textHeight = fontSize; // Aproximação, funciona bem com fontes padrão

	float textX = bounds.x + ( bounds.width - textWidth ) / 2.0f;
	float textY = bounds.y + ( bounds.height - textHeight ) / 2.0f;

	bool hovered = CheckCollisionPointRec( GetMousePosition( ) , bounds );
	bool pressed = hovered && IsMouseButtonPressed( MOUSE_LEFT_BUTTON );

	if ( hovered || pressed ) {
		DrawRectangleLinesEx( bounds , 1, color );
		DrawText( label , textX , textY , fontSize , color );
	}
	else {
		DrawRectangleRec( bounds , color );
		DrawText( label , textX , textY , fontSize , textColor );
	}

	return pressed;
}

bool components::DrawTextButton( Vector2 position , const char * text ,int fontSize, Color color , Color hoveredColor )
{
	Rectangle bounds = { position.x , position.y , ( float ) MeasureText( text , 20 ) , 20 };

	bool hovered = CheckCollisionPointRec( GetMousePosition( ) , bounds );
	bool pressed = hovered && IsMouseButtonPressed( MOUSE_LEFT_BUTTON );

	if ( pressed || hovered ) {
		DrawText( text , position.x , position.y , fontSize, hoveredColor );
	}
	else {
		DrawText( text , position.x , position.y , fontSize , color );
	}

	return pressed;
}


bool components::DrawSlider( Rectangle bounds , float * value , const char * label , float min , float max , Color backColor , Color frontColor ) {
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