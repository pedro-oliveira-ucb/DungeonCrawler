#include "RenderEntities.h"

#include "../../../Utils/Log/Log.h"

#include "../../World/World.h"
#include "../../../Globals/Globals.h"
#include "../../gameWorld/gameWorld.h"
#include "../../SDK/Entities/CPlayerEntity/CPlayerEntity.h"

#include <raylib/raylib.h>

#include <cmath>

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

void renderEntity( CBaseEntity * entity, bool DrawInfo = false ) {
    // Verifica se o jogador local existe
    if ( entity == nullptr ) {
        return;
    }

    CBaseEntityAnimation * entityAnimation = entity->getEntityAnimations( );

    if ( entityAnimation == nullptr )
        return;

    if ( entityAnimation->GetCurrentAnimation( ) == nullptr ) {
        return;
    }

    rSpriteAnimation * animation = entityAnimation->GetCurrentAnimation( );
    Texture2D * texture = static_cast< Texture2D * >( animation->getCurrentTexture( ) );
    GVector2D size = entityAnimation->getSpriteSize( );
    GVector2D pos = entity->getEntityPosition( );

    GAngle entityLookingAngle = entity->getLookingAngle( );
    GAngle entityMovingAngle = entity->getMovementAngle( );

    float baseAngle = 0.0f;

    switch ( entity->getEntityAnimations( )->getCurrentAnimationType( ) ) {
    case IDLE_FORWARD:
    case WALKING_FORWARD:
        baseAngle = 270.0f;
        break;
    case IDLE_BACKWARD:
    case WALKING_BACKWARD:
        baseAngle = 90.0f;
        break;
    case IDLE_LEFT:
    case WALKING_LEFT:
        baseAngle = 180.0f;
        break;
    case IDLE_RIGHT:
    case WALKING_RIGHT:
        baseAngle = 0.0f;
        break;
    default:
        baseAngle = 0.0f;
        break;
    }

    float lookingAngleDeg = entity->getLookingAngle( ).getDegrees( );
    float rotationAngle = AngleDiff( lookingAngleDeg , baseAngle ); // Isso garante um valor de -180° a 180°

  
    // Normaliza o ângulo para o intervalo [-180, 180]
    //if ( rotationAngle > 180.0f ) rotationAngle -= 360.0f;
    //if ( rotationAngle < -180.0f ) rotationAngle += 360.0f;

    Vector2 position = { pos.x, pos.y };
    Vector2 origin = { size.x / 2.0f, size.y / 2.0f };

    DrawTexturePro(
        *texture ,
        { 0, 0, ( float ) texture->width, ( float ) texture->height } ,
        { position.x, position.y, size.x, size.y } ,
        origin ,
        rotationAngle ,
        WHITE
    );

    // Corrigir a hitbox também para centralizar
    DrawRectangleLines(
        pos.x - size.x / 2.0f ,
        pos.y - size.y / 2.0f ,
        size.x ,
        size.y ,
        BLUE
    );

    if ( DrawInfo ) {

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


void RenderEntities::render( ) {
	renderEntity( _gameWorld.localplayer, true );
	for ( int i = 0; i < _gameWorld.entities.size( ); i++ ) {
		CBaseEntity * entity = _gameWorld.entities.at( i );
		if ( entity == nullptr ) {
			continue;
		}
		renderEntity( entity );
	}
}