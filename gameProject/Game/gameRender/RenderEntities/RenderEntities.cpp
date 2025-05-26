

#include "RenderEntities.h"

#include "../../Managers/LevelManager/LevelManager.h"

#include "../../../Utils/Log/Log.h"
#include "../../../Globals/Globals.h"
#include "../../gameObjects/attackHandler/attackHandler.h"
#include "../../gameObjects/entitiesHandler/entitiesHandler.h"
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

void renderEntity( CBaseEntity * entity , bool DrawInfo = false , float sizeFactor = 1.0f ) {
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

    float rotationAngle = 0.0f;
    float baseAngle = entity->getEntityLookingDirectionBaseAngle( );
    float lookingAngleDeg = entity->getLookingAngle( ).getDegrees( );

    switch ( entity->getEntityType( ) ) {
    case CBaseEntityType::ATTACK:
        rotationAngle = lookingAngleDeg - 90;
        break;
    default:
        rotationAngle = AngleDiff( lookingAngleDeg , baseAngle );
        break;
    }


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
        float offset = 0.0f;

		DrawText( entity->GetEntityName( ).c_str( ) , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );

		offset += 12.0f;

        DrawText( CBaseEntityAnimation::getAnimationTypeName(entity->getEntityAnimations( )->getCurrentAnimationType(  )).c_str() , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );


        offset += 12.0f;

        DrawText( std::to_string( entity->getEntityAnimations( )->getCurrentAnimationStep( ) ).c_str( ) , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );

        offset += 12.0f;

        DrawText( entity->getEntityStateAsString( ).c_str( ) , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );

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
    }
}

void renderAttacks( ) {
    int attacksSize = attackHandler::Get( ).runningAttacksSize( );
    for ( int i = 0; i < attacksSize; i++ ) {
        std::shared_ptr<CBaseAttack> attack = attackHandler::Get( ).getRunningAttack( i );

        if ( attack.get( ) == nullptr )
            continue;

        CBaseAttack * attackPtr = attack.get( );

        if ( attackPtr == nullptr )
            continue;

        if ( attackPtr->getEntityAnimations( ) == nullptr )
            continue;

        if ( !attackPtr->IsActive( ) )
            continue;

        renderEntity( attackPtr , true );
    }
}

void renderEnemies( ) {
    std::vector<std::shared_ptr<CEnemyEntity>> enemies = levelManager.getEnemies( );
    for ( int i = 0; i < enemies.size( ); i++ ) {
        std::shared_ptr<CEnemyEntity> enemy = enemies.at( i );

        if ( enemy.get( ) == nullptr )
            continue;

        CEnemyEntity * CEnemyPtr = enemy.get( );

        if ( CEnemyPtr == nullptr )
            continue;

        if ( CEnemyPtr->getEntityAnimations( ) == nullptr )
            continue;

        if ( !CEnemyPtr->isAlive( ) && CEnemyPtr->deathAnimationFinished( ) )
            continue;

        renderEntity( CEnemyPtr , true );
    }

}

void RenderEntities::render( ) {
    renderEntity( entitiesHandler::Get( ).getLocalPlayer( ) , true );
    renderAttacks( );
    renderEnemies( );
    /*for ( int i = 0; i < _gameWorld.entities.size( ); i++ ) {
        CBaseEntity * entity = _gameWorld.entities.at( i );
        if ( entity == nullptr ) {
            continue;
        }
        renderEntity( entity );
    }*/
}