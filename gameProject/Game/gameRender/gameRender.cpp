#include "gameRender.h"

#include "renderEntities/RenderEntities.h"
#include "renderDialogs/renderDialogs.h"
#include "renderPauseMenu/renderPauseMenu.h"
#include "renderCustomCursor/renderCustomCursor.h"
#include "renderGameOver/renderGameOver.h"
#include "renderMap/renderMap.h"
#include "renderShopMenu/renderShopMenu.h"
#include "renderHUD/renderHUD.h"

#include "../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../Managers/gameResourceManager/gameResourceManager.h"

#include "../../Utils/Log/Log.h"

renderDialogs dialogsRender;
renderEntities entitiesRender;
renderPauseMenu pauseMenuRender;
renderCustomCursor customCursorRender;
renderGameOver gameOverRender;
renderMap mapRender;
renderShopMenu shopMenuRender;
renderHUD hudRender;

void gameRender::processSoundEvents( ) {
	soundEvent event = gameSoundsEventHandler::Get( ).getLatestOnQueue( );
	while ( !event.soundName.empty( ) ) {

		if ( !_gameResourceManager.getSoundManager( )->playSound( event.soundName ) ) {
			Log::Print( "[soundEvents] cant play %s" , event.soundName.c_str( ) );
		}

		event = gameSoundsEventHandler::Get( ).getLatestOnQueue( );
	}
	_gameResourceManager.getMusicManager( )->updateMusic( );
}

void gameRender::renderDialogs( ){
	dialogsRender.render( );
}

void gameRender::renderPauseMenu( ){
	pauseMenuRender.render( );
}

void gameRender::renderCustomCursor( ) {
	customCursorRender.render( );
}

void gameRender::renderEntities( ) {
	entitiesRender.render( );
}

void gameRender::renderGameOver( ) {
	gameOverRender.render( );
}

void gameRender::renderMap( ) {
	mapRender.render( );
}

void gameRender::renderMapDoors( ) {
	mapRender.renderDoors( );
}

void gameRender::renderShopMenu( ) {
	shopMenuRender.render( );
}

void gameRender::renderHUD( ) {
	hudRender.render( );
}