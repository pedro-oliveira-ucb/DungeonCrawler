#include "RenderEntities.h"

#include <raylib/raylib.h>
#include <cmath>

#include "../../../Utils/Log/Log.h"

#include "../../gameObjects/attackHandler/attackHandler.h"

#include "../../World/World.h"
#include "../../../Globals/Globals.h"
#include "../../gameWorld/gameWorld.h"
#include "../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"

#define M_PI 3.14159265358979323846f

float angleDiff( const GVector2D & a , const GVector2D & b ) {
    float dot = a.x * b.x + a.y * b.y;
    float det = a.x * b.y - a.y * b.x; // Determinant (equivalente ao cross product em 2D)
    return atan2( det , dot ); // Retorna ângulo em radianos, entre -π e π
}

float angleDiff360( const GVector2D & a , const GVector2D & b ) {
    float angle = atan2( a.x * b.y - a.y * b.x , a.x * b.x + a.y * b.y );
    if ( angle < 0 ) angle += 2 * M_PI;
    return angle; // radianos
}



float AngleDiff( float a , float b );

void renderEntity( CBaseEntity * entity, bool DrawInfo = false, float sizeFactor = 1.0f ) {
    if ( entity == nullptr ) {
        return;
    }

    CBaseEntityAnimation * entityAnimation = entity->getEntityAnimations( );

    if ( entityAnimation == nullptr ) {
        return;
    }

    if ( entityAnimation->getCurrentTexture( ) == nullptr ) {
        return;
    }

    Texture2D * texture = static_cast< Texture2D * >( entityAnimation->getCurrentTexture( ) );
    GVector2D size = entityAnimation->getCurrentTextureSize( ) * sizeFactor;
    GVector2D pos = entity->getEntityPosition( );

    GAngle entityLookingAngle = entity->getLookingAngle( );
    GAngle entityMovingAngle = entity->getMovementAngle( );

    float baseAngle = entity->getEntityLookingDirectionBaseAngle();
    float lookingAngleDeg = entity->getLookingAngle( ).getDegrees( );
    float rotationAngle = AngleDiff( lookingAngleDeg , baseAngle );

    Vector2 position = { pos.x, pos.y };
    Vector2 origin = { size.x / 2.0f, size.y / 2.0f };

    DrawTexturePro(
        *texture ,
        { 0, 0, ( float ) texture->width, ( float ) texture->height } , // source = tamanho real da textura
        { position.x, position.y, size.x, size.y } ,               // dest = tamanho desejado (escalado)
        origin ,
        rotationAngle ,
        WHITE
    );

    if ( DrawInfo ) {
        DrawRectangleLines(
            pos.x - size.x / 2.0f ,
            pos.y - size.y / 2.0f ,
            size.x ,
            size.y ,
            BLUE
        );

        DrawRectangleLines(
            pos.x - size.x / 2 ,
            pos.y - size.y / 2 ,
            size.x ,
            size.y ,
            BLUE
        );

        //Moving angle
        DrawLine(
            pos.x ,
            pos.y ,
            pos.x + cosf( entityMovingAngle.getRadians( ) ) * 50 ,
            pos.y + sinf( entityMovingAngle.getRadians( ) ) * 50 ,
            RED
        );
        //Looking angle
        DrawLine(
            pos.x ,
            pos.y ,
            pos.x + cosf( entityLookingAngle.getRadians( ) ) * 50 ,
            pos.y + sinf( entityLookingAngle.getRadians( ) ) * 50 ,
            GREEN
        );

        DrawCircle(
            globals.mousePosX ,
            globals.mousePosY ,
            5 ,
            BLUE
        );
    }
}

void renderAttacks( ) {
    int attacksSize = attackHandler::Get().runningAttacksSize();
    for ( int i = 0; i < attacksSize; i++ ) {
		std::shared_ptr<CBaseAttack> attack = attackHandler::Get( ).getRunningAttack( i );

        if(attack.get() == nullptr )
			continue;
		CBaseAttack * attackPtr = attack.get( );

        if ( attackPtr == nullptr )
            continue;

		if ( attackPtr->getEntityAnimations( ) == nullptr )
			continue;

        renderEntity( attackPtr, true, 0.1 );
    }
}

void RenderEntities::render( ) {
	renderEntity( _gameWorld.localplayer, true );
	renderAttacks( );
	/*for ( int i = 0; i < _gameWorld.entities.size( ); i++ ) {
		CBaseEntity * entity = _gameWorld.entities.at( i );
		if ( entity == nullptr ) {
			continue;
		}
		renderEntity( entity );
	}*/
}