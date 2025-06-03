#include "renderMap.h"

#include "../../gameObjects/gameMap/gameMap.h"
#include "../../../Utils/Log/Log.h"

#include <raylib/raylib.h>


void renderMap::render( ) {
    auto tileSet = gameMap::Get( ).getCurrentTileSet( ); // Use referência
    if ( tileSet.empty( ) )
        return;

    auto tileObjects = gameMap::Get( ).getCurrentMapObjects( );
    if ( !tileObjects || tileObjects->empty( ) ) { // Adicionado nullptr check para tileObjects
        return;
    }

    GVector2D mapScreenPosition = gameMap::Get( ).getMapPosition( ); // Posição inicial do mapa na tela

    float currentScreenY = mapScreenPosition.y;

    for ( size_t i = 0; i < tileSet.size( ); ++i ) { // Usar size_t para índices de vetor
        const auto & row = tileSet[ i ]; // Referência à linha atual
        if ( row.empty( ) ) {
            continue;
        }

        float currentScreenX = mapScreenPosition.x;
        float lineHeight = 0.0f; // Para armazenar a altura da linha atual (altura do primeiro tile)

        for ( size_t j = 0; j < row.size( ); ++j ) {
            mapObjectType tileType = row[ j ];

            if ( tileObjects->find( tileType ) == tileObjects->end( ) || !( *tileObjects )[ tileType ] ) {             
                continue;
            }

            auto spriteWrapper = ( *tileObjects )[ tileType ]->getSprite( );
            if ( !spriteWrapper || !spriteWrapper->get( ) ) {            
                continue;
            }

            void * textureAddress = spriteWrapper->get( )->getTexture( );
            if ( textureAddress == nullptr ) {       
                continue;
            }

            GVector2D tileSize = spriteWrapper->get( )->getSpriteSize( );
            Texture2D * tileTexture = reinterpret_cast< Texture2D * >( textureAddress );

            if ( j == 0 ) { // Primeiro tile da linha define a altura da linha
                lineHeight = tileSize.y;
            }


            DrawTexture( *tileTexture , static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , WHITE );

            currentScreenX += tileSize.x; // Avança X pela largura do tile desenhado
        }

   
        if ( lineHeight > 0 ) { // Apenas avança se a linha teve tiles (e portanto uma altura)
            currentScreenY += lineHeight;
        }
        else if ( !row.empty( ) ) {
         
        }
    }
}