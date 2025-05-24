#include "gameRender.h"

#include "../../Utils/Log/Log.h"

#include "../../Globals/Globals.h"

#include "RenderEntities/RenderEntities.h"

#include "../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../gameObjects/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../Managers/gameResourceManager/gameResourceManager.h"

#include <SFML/Graphics.hpp>

RenderEntities entititiesRender;

bool check = false;

void gameRender::soundEvents( ) {

	std::string soundEvent = gameSoundsQueue.getLatestOnQueue( );
	while ( !soundEvent.empty( ) ) {
		if ( !_gameResourceManager.getSoundManager( )->playSound( soundEvent ) ) {
			Log::Print( "[soundEvents] cant play %s" , soundEvent.c_str( ) );
		}
		soundEvent = gameSoundsQueue.getLatestOnQueue( );
	}
}

void gameRender::renderCustomCursor( ) {
	//DrawCircle( GetMouseX( ) , GetMouseY( ) , 10 , BLUE );
}

GVector2D oldLocalPos;

void gameRender::render( sf::RenderWindow & window ) {
    static float zoomLevel = 1.0f;
    static GVector2D oldLocalPos;

    // Mouse scroll (zoom)
    float deltaZoom = globals.mouseWheelDelta * 0.1f;
    zoomLevel += deltaZoom;
    if ( zoomLevel < 1.0f ) zoomLevel = 1.0f;
    if ( zoomLevel > 2.0f ) zoomLevel = 2.0f;

    CPlayerEntity * local = entitiesHandler::Get( ).getLocalPlayer( );
    if ( local != nullptr ) {
        GVector2D localPos = local->getEntityPosition( );

        //sf::Vector2f mousePos( globals.mousePosX , globals.mousePosY );

        //// Corrige a posição do mouse com base no movimento do jogador
        //if ( oldLocalPos.x != localPos.x || oldLocalPos.y != localPos.y ) {
        //    float deltaX = localPos.x - oldLocalPos.x;
        //    float deltaY = localPos.y - oldLocalPos.y;

        //    mousePos.x += deltaX;
        //    mousePos.y += deltaY;

        //    oldLocalPos = localPos;
        //}

        //// Corrige limites da posição do mouse
        //mousePos.x = std::clamp( mousePos.x , localPos.x - ( globals.screenWidth / 2.0f ) , localPos.x + ( globals.screenWidth / 2.0f ) );
        //mousePos.y = std::clamp( mousePos.y , localPos.y - ( globals.screenHeight / 2.0f ) , localPos.y + ( globals.screenHeight / 2.0f ) );

        //// Define posição do mouse (precisa de código específico para SFML + OS, como usando sf::Mouse::setPosition)
        //sf::Vector2i screenMousePos( static_cast< int >( mousePos.x ) , static_cast< int >( mousePos.y ) );
        //sf::Mouse::setPosition( screenMousePos , window ); // CUIDADO: isso move o mouse em tela, pode ser irritante

        // Atualiza a câmera
        sf::View view;
        view.setCenter( { localPos.x , localPos.y } );
        view.setSize( { globals.screenWidth / zoomLevel , globals.screenHeight / zoomLevel } );

   
        window.setView( view );
    }

    window.clear( );

    // Renderização de entidades
    entititiesRender.render( ); // Certifique-se de passar `window` para desenhar corretamente

    // Render cursor customizado
    renderCustomCursor( ); // Também precisa da window para desenhar

    window.display( );
}
