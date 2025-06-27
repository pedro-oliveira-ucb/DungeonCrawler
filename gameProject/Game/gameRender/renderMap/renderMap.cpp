#include "renderMap.h"
#include <unordered_map>

#include "../../../Globals/Globals.h"

#include "../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../gameObjects/CameraController/CameraController.h"
#include "../../gameObjects/gameMap/gameMap.h"
#include "../../../Utils/Log/Log.h"

#include <raylib/raylib.h>


void renderTilesetEmGrid( const TileSet & tileSet ,
	std::unordered_map<mapObjectType , std::shared_ptr<rMapObject>> * tileObjects ,
	const GVector2D & mapScreenPosition 
) {
	const float TILE_WIDTH = gameMap::Get( ).getTileDimensions( ).x;
	const float TILE_HEIGHT = gameMap::Get( ).getTileDimensions( ).y;

	// Se o tileset ou os tiles estiverem vazios, não há nada a fazer.
	if ( tileSet.empty( ) || TILE_WIDTH <= 0 || TILE_HEIGHT <= 0 ) {
		return;
	}

	// Obtenha as informações globais UMA VEZ para evitar chamadas repetidas.
	const auto & globals = Globals::Get( );
	const float screenWidth = globals.screenWidth;
	const float screenHeight = globals.screenHeight;

	// Achar o número máximo de colunas.
	// previamente corrigido no map generator, todos tem a mesma quantidade de colunas.
	size_t maxCols = tileSet.at( 0 ).size( );

	Vector2 localPosition = CameraController::Get( ).GetCameraPosition( );
	localPosition.x -= screenWidth / 2.0f; // Centraliza a câmera na tela
	localPosition.y -= screenHeight / 2.0f; // Centraliza a câmera na tela

	// Calcule quais colunas (tiles na horizontal) estão visíveis na tela.
	size_t startCol = std::max( 0 , static_cast< int >( localPosition.x / TILE_WIDTH ) );
	size_t endCol = std::min( maxCols , static_cast< size_t >( ( localPosition.x + screenWidth ) / TILE_WIDTH ) + 2 ); // +2 de margem

	// Calcule quais linhas (tiles na vertical) estão visíveis na tela.
	size_t startRow = std::max( 0 , static_cast< int >( localPosition.y / TILE_HEIGHT ) );
	size_t endRow = std::min( tileSet.size( ) , static_cast< size_t >( ( localPosition.y + screenHeight * 2 ) / TILE_HEIGHT ) + 2 ); // +2 de margem

	for ( size_t row = startRow; row < endRow; ++row ) {
		const auto & currentRow = tileSet[ row ];

		for ( size_t col = startCol; col < endCol; ++col ) {
			// Verifica se a coluna existe nesta linha (para mapas com linhas de tamanhos diferentes)
			if ( col >= currentRow.size( ) ) {
				continue;
			}

			mapObjectType tileType = currentRow[ col ];

			switch ( tileType ) {
			case mapObjectType::mapObjectNone:
				continue; // Pula tiles vazios
			case mapObjectType::door:
			case mapObjectType::unlocked_door:
				// Se encontrar uma porta, renderiza o chão embaixo primeiro.
				// (Esta lógica pode ser ajustada conforme a necessidade do seu jogo)
				tileType = mapObjectType::ground;
				break;
			}

			// Acesso otimizado ao mapa: procure uma vez e reutilize o resultado.
			auto it = tileObjects->find( tileType );
			if ( it == tileObjects->end( ) || !it->second ) {
				continue;
			}

			// O resto da lógica para obter a textura.
			std::shared_ptr<rSprite> * spriteWrapper = it->second->getSprite( );
			if ( !spriteWrapper || !spriteWrapper->get( ) ) {
				continue;
			}

			void * textureAddress = spriteWrapper->get( )->getTexture( );
			if ( !textureAddress ) {
				continue;
			}

			Texture2D * tileTexture = reinterpret_cast< Texture2D * >( textureAddress );

			// --- 4. CÁLCULO DE POSIÇÃO E DESENHO ---
			// A posição do tile na tela é calculada de forma direta e simples.
			const float currentScreenX = col * TILE_WIDTH + mapScreenPosition.x;
			const float currentScreenY = row * TILE_HEIGHT + mapScreenPosition.y;
			const int currentGameMapID = gameMap::Get( ).getRoomIdAtPosition( GVector2D( currentScreenX , currentScreenY ) );

			if ( tileTexture->width < TILE_WIDTH || tileTexture->height < TILE_HEIGHT ) {
				float tempWidth = 0.0f;
				do {
					float tempHeight = 0.0f;
					do {
						// Desenha a textura do tile na posição correta.
						DrawTexture( *tileTexture , static_cast< int >( currentScreenX + tempWidth ) , static_cast< int >( currentScreenY + tempHeight ) , WHITE );
						tempHeight += tileTexture->height;
					} while( tempHeight < TILE_HEIGHT );

					tempWidth += tileTexture->width;
				} while ( tempWidth < TILE_WIDTH );
			}
			else {
				// Desenha a textura do tile na posição correta.
				DrawTexture( *tileTexture , static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , WHITE );
			}

#if _DEBUG
			// Lógica de debug para ver os índices, se necessário.
			std::string id = std::to_string( currentGameMapID );
			DrawText( id.c_str( ) , static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , 8 , WHITE );
#endif
		}
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
	renderTilesetEmGrid( tileSet , tileObjects , mapScreenPosition );
}

void renderMap::renderDoors( ) {
	mapType type = gameMap::Get( ).getCurrentMapType( );

	std::map<GVector2D , DoorInstanceData > doors = gameMap::Get( ).getDoorInstancesCopy( );

	if ( doors.empty( ) ) {
		Log::Print( "No doors to render." );
	}

	CPlayerEntity * player = entitiesHandler::Get( ).getLocalPlayer( );

	GVector2D playerPosition = player ? player->getEntityPosition( ) : GVector2D( 0.0f , 0.0f );

	float currentTime = Globals::Get( ).getGame( )->getCurrentGameTime( );

	for ( auto it = doors.begin( ); it != doors.end( ); ++it ) {
		GVector2D position = it->first;

		// Verifica se a porta está desbloqueada
		mapObjectType doorType = it->second.unlocked ? mapObjectType::unlocked_door : mapObjectType::door;

		std::shared_ptr<rMapObject> * mapObject = mapObjectsHandler::Get( ).getGameObject( type , doorType );

		if ( !mapObject )
			continue;

		void * spriteAddress = mapObject->get( )->getSprite( )->get( )->getTexture( );

		if ( spriteAddress == nullptr )
			continue;

		Texture2D * texture = reinterpret_cast< Texture2D * >( spriteAddress );

		if ( texture == nullptr )
			continue;

		DrawTexture( *texture , static_cast< int >( position.x ) , static_cast< int >( position.y ) , WHITE );

		if ( position.y > playerPosition.y ) {
			continue;
		}

		if ( position.distTo( playerPosition ) <= 100 && doorType != mapObjectType::unlocked_door ) {
			float pulseSpeed = 2.5f;   // Velocidade da pulsação (ciclos por segundo)
			float minAlphaPulse = 0.3f;  // Opacidade mínima da pulsação (0.0 a 1.0)
			float maxAlphaPulse = 0.7f;  // Opacidade máxima da pulsação (0.0 a 1.0)

			// Calcula o alfa (transparência) baseado em uma onda senoidal para pulsação suave
			float alphaNormalized = ( sin( currentTime * PI * pulseSpeed ) + 1.0f ) / 2.0f; // Normalizado entre 0.0 e 1.0
			float auraAlpha = minAlphaPulse + ( maxAlphaPulse - minAlphaPulse ) * alphaNormalized;

			Color auraColor = { 173, 216, 230, static_cast< unsigned char >( auraAlpha * 255 ) }; // Azul Claro (LightBlue)

			// Calcula o centro e o raio para a aura.
			// A aura pode ser um círculo ou um retângulo ligeiramente maior que a porta.
			// Vamos usar um círculo centrado na porta.
			GVector2D doorCenter = GVector2D( position.x + texture->width / 2.0f , position.y + texture->height / 2.0f );
			// O raio da aura pode ser baseado na maior dimensão da textura da porta, com um pequeno extra.
			float baseRadius = ( texture->width > texture->height ? texture->width : texture->height ) * 0.6f;
			float auraRadius = baseRadius + 10.0f * alphaNormalized; // Raio também pode pulsar um pouco

			// Desenha a aura. Usando DrawCircleGradient para uma borda mais suave.
			// O gradiente vai de uma cor mais opaca no centro para transparente na borda.
			DrawCircleGradient(
				static_cast< int >( doorCenter.x ) ,
				static_cast< int >( doorCenter.y ) ,
				auraRadius ,
				auraColor ,                                  // Cor interna (parte mais opaca do gradiente)
				Fade( auraColor , 0.0f )                       // Cor externa (totalmente transparente)
			);

			//Texto de "Desbloquear"
			//Texto claro e elegante indicando a ação e a tecla.
			const char * unlockText = "Desbloquear [E]";
			int fontSize = 18; 
			Color textColor = { 245, 245, 245, 240 }; // Um branco levemente acinzentado (WhiteSmoke), quase opaco

			// Calcula as dimensões do texto para posicionamento
			// Assumindo que existe MeasureText(const char *text, int fontSize)
			int textWidth = MeasureText( unlockText , fontSize );

			// Posiciona o texto: centralizado horizontalmente abaixo da porta, com algum espaçamento.
			int textPosX = static_cast< int >( doorCenter.x - textWidth / 2.0f );
			int textPosY = static_cast< int >( position.y + texture->height + 12 ); // 12 pixels abaixo da textura da porta

			// Adicionar uma sombra sutil ou fundo ao texto para melhor legibilidade
			DrawText(unlockText, textPosX + 1, textPosY + 1, fontSize, Fade(BLACK, 0.6f)); // Sombra simples
			// Desenha o texto principal
			DrawText( unlockText , textPosX , textPosY , fontSize , textColor );
		}
	}
}