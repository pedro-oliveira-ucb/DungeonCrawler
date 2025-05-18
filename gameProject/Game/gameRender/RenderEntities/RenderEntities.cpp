#include "RenderEntities.h"

#include "../../../Utils/Log/Log.h"

#include "../../World/World.h"
#include "../../gameWorld/gameWorld.h"
#include "../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"

#include <raylib/raylib.h>

bool did = false;

void renderLocalPlayer( ) {
    GVector2D pos = _gameWorld.localplayer->getEntityPosition( );
    
	// Verifica se o jogador local existe
    if ( _gameWorld.localplayer == nullptr ) {
		Log::Print( "add" );
		return;
    }

   

    CBaseEntityAnimation * localPlayerAnimation = _gameWorld.localplayer->getEntityAnimations( );

    if ( localPlayerAnimation == nullptr )
        return;

	if ( !did ) {
		Log::Print( "[render] Animation Address: %p", localPlayerAnimation->GetCurrentAnimation( ) );
        did = true;
		return;
	}

    if ( localPlayerAnimation->GetCurrentAnimation( ) == nullptr) {
        return;
    }

	rSpriteAnimation * animation = localPlayerAnimation->GetCurrentAnimation( );

    Texture2D * texture = static_cast< Texture2D * >( animation->getCurrentTexture( ) );

    GVector2D size = localPlayerAnimation->getSpriteSize( );

    DrawTexture( *texture , pos.x , pos.y , WHITE );
}


void RenderEntities::render( ) {
	renderLocalPlayer( );
}