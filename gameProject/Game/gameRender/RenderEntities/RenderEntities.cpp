#include "RenderEntities.h"
#include <cmath>
#include "../../Managers/LevelManager/LevelManager.h"
#include "../../../Utils/Log/Log.h"
#include "../../../Globals/Globals.h"
#include "../../gameObjects/attackHandler/attackHandler.h"
#include "../../gameObjects/entitiesHandler/entitiesHandler.h"
#include <SFML/Graphics.hpp>


using namespace sf;

// Adicione estas declarações/externs se necessário
extern sf::Font g_font; // Fonte global precisa ser carregada previamente

float angleDiff( const GVector2D & a , const GVector2D & b ) {
    float dot = a.x * b.x + a.y * b.y;
    float det = a.x * b.y - a.y * b.x;
    return atan2( det , dot );
}

float angleDiff360( const GVector2D & a , const GVector2D & b ) {
    float angle = atan2( a.x * b.y - a.y * b.x , a.x * b.x + a.y * b.y );
    if ( angle < 0 ) angle += 2 * static_cast< float >( M_PI );
    return angle;
}

float AngleDiff( float a , float b );

// Parâmetros corrigidos com ordem correta
void renderEntity( CBaseEntity * entity , sf::RenderWindow * window , bool DrawInfo = false , float sizeFactor = 1.0f ) {
    if ( !entity || !window ) return;

    CBaseEntityAnimation * entityAnimation = entity->getEntityAnimations( );
    if ( !entityAnimation ) return;

    sf::Texture * texture = static_cast< sf::Texture * >( entityAnimation->getCurrentTexture( ) );
    if ( !texture ) return;

    GVector2D size = entityAnimation->getCurrentTextureSize( ) * sizeFactor;
    GVector2D pos = entity->getEntityPosition( );

    // Configuração do sprite
    sf::Sprite sprite( *texture ); // Usando construtor com textura

    // Definindo origem e posição com conversão explícita para sf::Vector2f
    sprite.setOrigin( sf::Vector2f( size.x / 2.0f , size.y / 2.0f ) );
    sprite.setPosition( sf::Vector2f( pos.x , pos.y ) );

    // Cálculo de rotação
    float rotationAngle = 0.0f;
    switch ( entity->getEntityType( ) ) {
    case CBaseEntityType::ATTACK:
        rotationAngle = entity->getLookingAngle( ).getDegrees( ) - 90.0f;
        break;
    default:
        rotationAngle = AngleDiff( entity->getLookingAngle( ).getDegrees( ) ,
            entity->getEntityLookingDirectionBaseAngle( ) );
        break;
    }

	sprite.rotate( sf::degrees( rotationAngle ) ); // Corrigido para degrees

    // Escala baseada no fator de tamanho
    sf::Vector2u texSize = texture->getSize( );
    if ( texSize.x > 0 && texSize.y > 0 ) {
        sprite.setScale(
            sf::Vector2f( ( size.x / static_cast< float >( texSize.x ) ) * sizeFactor ,
                ( size.y / static_cast< float >( texSize.y ) ) * sizeFactor )
        );
    }

    window->draw( sprite );

    if ( DrawInfo ) {
        // Desenhar informações de debug
        //float offSet = 0.0f;

        //// Texto do nome
        //sf::Text text(g_font);
        //text.setFont( g_font );
        //text.setCharacterSize( 12 );
        //text.setFillColor( sf::Color::Black );

        //// Nome da entidade
        //text.setString( entity->GetEntityName( ) );
        //text.setPosition( sf::Vector2f( pos.x - size.x / 2.0f , pos.y + size.y / 2.0f + offSet ) );
        //window->draw( text );
        //offSet += 12.0f;

        //// Tipo de animação
        //text.setString( entityAnimation->getAnimationTypeName(
        //    entityAnimation->getCurrentAnimationType( ) ) );
        //text.setPosition( sf::Vector2f( pos.x - size.x / 2.0f , pos.y + size.y / 2.0f + offSet ) );
        //window->draw( text );

        //// Borda azul
        //sf::RectangleShape border( sf::Vector2f( size.x , size.y ) );
        //border.setPosition( sf::Vector2f( pos.x - size.x / 2.0f , pos.y - size.y / 2.0f ) );
        //border.setFillColor( sf::Color::Transparent );
        //border.setOutlineColor( sf::Color::Blue );
        //border.setOutlineThickness( 1.0f );
        //window->draw( border );

        //// Linhas de direção
        //const float lineLength = 50.0f;
        //GAngle movingAngle = entity->getMovementAngle( );
        //GAngle lookingAngle = entity->getLookingAngle( );


        //// Movimento (vermelho)
        //sf::VertexArray movingLine( sf:: , 2 );
        //movingLine[ 0 ].position = sf::Vector2f( pos.x , pos.y );
        //movingLine[ 1 ].position = sf::Vector2f(
        //    pos.x + cosf( movingAngle.getRadians( ) ) * lineLength ,
        //    pos.y + sinf( movingAngle.getRadians( ) ) * lineLength
        //);
        //movingLine[ 0 ].color = sf::Color::Red;
        //movingLine[ 1 ].color = sf::Color::Red;
        //window->draw( movingLine );

        //// Visão (verde)
        //sf::VertexArray lookingLine( sf::Lines , 2 );
        //lookingLine[ 0 ].position = sf::Vector2f( pos.x , pos.y );
        //lookingLine[ 1 ].position = sf::Vector2f(
        //    pos.x + cosf( lookingAngle.getRadians( ) ) * lineLength ,
        //    pos.y + sinf( lookingAngle.getRadians( ) ) * lineLength
        //);
        //lookingLine[ 0 ].color = sf::Color::Green;
        //lookingLine[ 1 ].color = sf::Color::Green;
        //window->draw( lookingLine );
    }
}

// Funções restantes com ajustes de chamada
void renderAttacks( sf::RenderWindow * window ) {
    for ( int i = 0; i < attackHandler::Get( ).runningAttacksSize( ); i++ ) {
        auto attack = attackHandler::Get( ).getRunningAttack( i );
        if ( attack && attack->IsActive( ) ) {
            renderEntity( attack.get( ) , window , true );
        }
    }
}

void renderEnemies( sf::RenderWindow * window ) {
    for ( auto & enemy : levelManager.getEnemies( ) ) {
        if ( enemy && enemy->isAlive( ) && !enemy->deathAnimationFinished( ) ) {
            renderEntity( enemy.get( ) , window , true );
        }
    }
}

void RenderEntities::render( ) {
    if ( !globals.windowPointer ) return;

    sf::RenderWindow * window = static_cast< sf::RenderWindow * >( globals.windowPointer );

    if ( auto player = entitiesHandler::Get( ).getLocalPlayer( ) ) {
        renderEntity( player , window , true );
    }

    renderAttacks( window );
    renderEnemies( window );
}