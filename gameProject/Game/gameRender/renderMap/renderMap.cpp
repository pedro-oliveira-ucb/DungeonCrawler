#include "renderMap.h"
#include <unordered_map>

#include "../../gameObjects/gameMap/gameMap.h"
#include "../../../Utils/Log/Log.h"

#include <raylib/raylib.h>


void renderTilesetEmGrid( std::vector<DoorMapObject> * doors ,
	const TileSet & tileSet , std::unordered_map<mapObjectType ,
	std::shared_ptr<rMapObject>> *tileObjects ,
	const GVector2D & mapScreenPosition
) {
	std::unordered_map< mapObjectType , GVector2D> spriteAreaInCells;

	bool doorsEmpty = doors->empty( );

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
				if ( !row )
				{
					rowWidth = tileSize.x; // Define a altura da linha com base no primeiro tile
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
			else if ( tileType != mapObjectType::door ) {
				DrawTexture( *tileTexture , static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , WHITE );
			}
			else if ( doorsEmpty ) {
				DoorMapObject door;
				door.position = GVector2D( static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) );
				door.unlocked = false;

				doors->emplace_back( door ); // Adiciona a porta à lista de portas
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
	renderTilesetEmGrid( &this->doors, tileSet , tileObjects , mapScreenPosition );
}

void renderMap::renderDoors( ) {
	if ( this->doors.empty( ) )
		return;

	mapType type = gameMap::Get( ).getCurrentMapType( );

	std::shared_ptr<rMapObject> * mapObject = mapObjectsHandler::Get( ).getGameObject( type , mapObjectType::door );

	if ( !mapObject )
		return;

	void * spriteAddress = mapObject->get( )->getSprite( )->get( )->getTexture( );

	if ( spriteAddress == nullptr )
		return;

	Texture2D * texture = reinterpret_cast< Texture2D * >( spriteAddress );

	if ( texture == nullptr )
		return;

	for ( const auto & door : this->doors ) {
		DrawTexture( *texture , static_cast< int >( door.position.x ) , static_cast< int >( door.position.y ) , WHITE );
	}

}