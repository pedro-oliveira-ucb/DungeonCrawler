#include "renderMap.h"
#include <unordered_map>

#include "../../gameObjects/gameMap/gameMap.h"
#include "../../../Utils/Log/Log.h"

#include <raylib/raylib.h>


void renderTilesetEmGrid(
	const TileSet & tileSet ,
	std::unordered_map<mapObjectType , std::shared_ptr<rMapObject>> * tileObjects ,
	const GVector2D & mapScreenPosition
) {

	std::unordered_map< mapObjectType , GVector2D> spriteAreaInCells;

	// 1) Descobre a área (em células) de cada sprite de mapObjectType
	for ( const auto & pair : *tileObjects ) {
		mapObjectType tileType = pair.first;
		auto mapObject = pair.second;
		if ( mapObject ) {
			auto sprite = mapObject->getSprite( );
			if ( sprite && sprite->get( ) ) {
				// Calcula a área do sprite em células (16px cada)
				GVector2D area;
				GVector2D spriteSize = sprite->get( )->getSpriteSize( );
				area.x = static_cast< int >( spriteSize.x / 16 ); // Largura em células
				area.y = static_cast< int >( spriteSize.y / 16 ); // Largura em células
				spriteAreaInCells[ tileType ] = area;
			}
		}
	}

	size_t maxCols = 0;
	for ( const auto & row : tileSet ) {
		if ( row.size( ) > maxCols ) {
			maxCols = row.size( );
		}
	}

	float columnHeight = 0.0f;
	float rowWidth = 0.0f;
	float currentScreenX = mapScreenPosition.x; // ou outra lógica

	// 2. Iterar por colunas, depois por linhas
	for ( size_t col = 0; col < maxCols; ++col ) {
		float currentScreenY = mapScreenPosition.y;

		for ( size_t row = 0; row < tileSet.size( ); ++row ) {
			const auto & currentRow = tileSet[ row ];

			// Verifica se essa linha tem essa coluna
			if ( col >= currentRow.size( ) ) {
				continue;
			}

			mapObjectType tileType = currentRow[ col ];
			if ( tileType == mapObjectType::mapObjectNone ) {
				currentScreenY += columnHeight; // empilha verticalmente os tiles da coluna
				continue;
			}

			if ( tileObjects->find( tileType ) == tileObjects->end( ) || !( *tileObjects )[ tileType ] ) continue;

			auto spriteWrapper = ( *tileObjects )[ tileType ]->getSprite( );
			if ( !spriteWrapper || !spriteWrapper->get( ) ) continue;

			void * textureAddress = spriteWrapper->get( )->getTexture( );
			if ( !textureAddress ) continue;

			GVector2D tileSize = spriteWrapper->get( )->getSpriteSize( );
			Texture2D * tileTexture = reinterpret_cast< Texture2D * >( textureAddress );

			if ( !col ) {
				columnHeight = tileSize.y; // Define a largura da coluna com base no primeiro tile
				DrawRectangle( static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , static_cast< int >( tileSize.x ) , static_cast< int >( tileSize.y ) , Fade( RED , 0.5f ) );
				if ( !row )
				{
					rowWidth = tileSize.x; // Define a altura da linha com base no primeiro tile
					DrawRectangle( static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , static_cast< int >( tileSize.x ) , static_cast< int >( tileSize.y ) , Fade( BLUE , 0.5f ) );
				}
			}


			if ( tileSize.x < rowWidth ) {
				float tempWidth = 0.0f;
				while ( tempWidth < rowWidth ) {
					float tempHeight = 0.0f; // Posição Y atual para empilhar os tiles
					while ( tempHeight < columnHeight ) {
						DrawTexture( *tileTexture , static_cast< int >( currentScreenX + tempWidth ) , static_cast< int >( currentScreenY + tempHeight ) , WHITE );
						tempHeight += tileSize.y; // empilha verticalmente os tiles da coluna
					}
					tempWidth += tileSize.x; // Ajusta a posição X para alinhar com a largura da coluna
				}
			}
			else {
				DrawTexture( *tileTexture , static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , WHITE );
			}


			currentScreenY += columnHeight; // empilha verticalmente os tiles da coluna
		}

		currentScreenX += rowWidth;
	}
}


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

	/*
	mapTileSet tile =
	{
		{lt,t,t,t,d,t,t,t,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,g,g,g,g,g,g,g,lt},
		{lt,t,t,t,t,t,t,t,lt}
	};
	*/

	renderTilesetEmGrid( tileSet , tileObjects , mapScreenPosition );
}