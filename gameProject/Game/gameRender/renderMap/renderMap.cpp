#include "renderMap.h"
#include <unordered_map>

#include "../../Handlers/entitiesHandler/entitiesHandler.h"

#include "../../gameObjects/gameMap/gameMap.h"
#include "../../../Utils/Log/Log.h"

#include <raylib/raylib.h>


void renderTilesetEmGrid( const TileSet & tileSet , std::unordered_map<mapObjectType ,
	std::shared_ptr<rMapObject>> *tileObjects ,
	const GVector2D & mapScreenPosition
) {

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

			switch ( tileType ) {
			case mapObjectType::mapObjectNone:
				currentScreenY += columnHeight; // empilha verticalmente os tiles da coluna
				continue; // Ignora tiles que não são objetos ou portas
			case mapObjectType::door:
			case mapObjectType::unlocked_door:
				// renderiza o chao embaixo da porta
				tileType = mapObjectType::ground;
				break;
			}

			if ( tileObjects->find( tileType ) == tileObjects->end( ) || !( *tileObjects )[ tileType ] )
				continue;

			std::shared_ptr<rSprite> * spriteWrapper = ( *tileObjects )[ tileType ]->getSprite( );

			if ( !spriteWrapper || !spriteWrapper->get( ) )
				continue;

			void * textureAddress = spriteWrapper->get( )->getTexture( );
			if ( !textureAddress )
				continue;

			GVector2D tileSize = spriteWrapper->get( )->getSpriteSize( );
			Texture2D * tileTexture = reinterpret_cast< Texture2D * >( textureAddress );


			if ( !columnHeight && ( tileSize.x > 0 && tileSize.y > 0 ) ) {
				columnHeight = tileSize.y; // Define a largura da coluna com base no primeiro tile	
				currentScreenY += (columnHeight * col); // Ajusta a posição Y inicial para o primeiro tile
			}

			if ( !rowWidth && ( tileSize.x > 0 && tileSize.y > 0 ) ) {
				rowWidth = tileSize.x; // Define a altura da linha com base no primeiro tile
			}

			// Verifica se a largura da coluna é maior que a largura do tile
			if ( tileSize.x < rowWidth ) {
				float tempWidth = 0.0f;
				do {
					float tempHeight = 0.0f; // Posição Y atual para empilhar os tiles
					do {
						DrawTexture( *tileTexture , static_cast< int >( currentScreenX + tempWidth ) , static_cast< int >( currentScreenY + tempHeight ) , WHITE );
						tempHeight += tileSize.y; // empilha verticalmente os tiles da coluna
					} while ( tempHeight < columnHeight );
					tempWidth += tileSize.x; // Ajusta a posição X para alinhar com a largura da coluna

				} while ( tempWidth < rowWidth );
			}
			else {
				DrawTexture( *tileTexture , static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , WHITE );
			}

			std::string id = std::to_string( gameMap::Get( ).getRoomIdAtPosition( GVector2D( currentScreenX , currentScreenY ) ) );
			DrawText( id.c_str( ) , static_cast< int >( currentScreenX ) , static_cast< int >( currentScreenY ) , 10 , WHITE );

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

	for ( auto it = doors.begin( ); it != doors.end( ); ++it ) {
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

		GVector2D position = it->first;

		DrawTexture( *texture , static_cast< int >( position.x ) , static_cast< int >( position.y ) , WHITE );

		if ( position.distTo( playerPosition ) <= 100 && doorType != mapObjectType::unlocked_door ) {
			// 1. Aura Pulsante ao Redor da Porta
			// Cria um brilho suave e convidativo que pulsa gentilmente.

			// Obter o tempo atual (assumindo uma função GetTime() que retorna segundos)
			float currentTime = GetTime( ); // Adapte para a sua engine/biblioteca
			float pulseSpeed = 2.5f;   // Velocidade da pulsação (ciclos por segundo)
			float minAlphaPulse = 0.3f;  // Opacidade mínima da pulsação (0.0 a 1.0)
			float maxAlphaPulse = 0.7f;  // Opacidade máxima da pulsação (0.0 a 1.0)

			// Calcula o alfa (transparência) baseado em uma onda senoidal para pulsação suave
			float alphaNormalized = ( sin( currentTime * PI * pulseSpeed ) + 1.0f ) / 2.0f; // Normalizado entre 0.0 e 1.0
			float auraAlpha = minAlphaPulse + ( maxAlphaPulse - minAlphaPulse ) * alphaNormalized;

			// Cor para a aura (ex: um azul místico ou um dourado acolhedor)
			// Usando uma estrutura Color similar à da Raylib: {R, G, B, A} onde A é 0-255.
			// Convertendo o auraAlpha (0-1) para a faixa 0-255.
			Color auraColor = { 173, 216, 230, static_cast< unsigned char >( auraAlpha * 255 ) }; // Azul Claro (LightBlue)
			// Alternativa: Color auraColor = { 255, 223, 100, static_cast<unsigned char>(auraAlpha * 255) }; // Dourado Quente

			// Calcula o centro e o raio para a aura.
			// A aura pode ser um círculo ou um retângulo ligeiramente maior que a porta.
			// Vamos usar um círculo centrado na porta.
			GVector2D doorCenter = GVector2D( position.x + texture->width / 2.0f , position.y + texture->height / 2.0f );
			// O raio da aura pode ser baseado na maior dimensão da textura da porta, com um pequeno extra.
			float baseRadius = ( texture->width > texture->height ? texture->width : texture->height ) * 0.6f;
			float auraRadius = baseRadius + 10.0f * alphaNormalized; // Raio também pode pulsar um pouco

			// Desenha a aura. Usando DrawCircleGradient para uma borda mais suave.
			// Se DrawCircleGradient não estiver disponível, um DrawCircle com a auraColor também funcionaria.
			// O gradiente vai de uma cor mais opaca no centro para transparente na borda.
			DrawCircleGradient(
				static_cast< int >( doorCenter.x ) ,
				static_cast< int >( doorCenter.y ) ,
				auraRadius ,
				auraColor ,                                  // Cor interna (parte mais opaca do gradiente)
				Fade( auraColor , 0.0f )                       // Cor externa (totalmente transparente)
			);
			// Alternativas mais simples:
			// DrawCircle(static_cast<int>(doorCenter.x), static_cast<int>(doorCenter.y), auraRadius, auraColor);
			// DrawRectangle(static_cast<int>(position.x - 5), static_cast<int>(position.y - 5), texture->width + 10, texture->height + 10, auraColor);

			// 2. Texto de "Desbloquear"
			// Texto claro e elegante indicando a ação e a tecla.
			const char * unlockText = "Desbloquear [E]"; // Ou "Abrir [Espaço]", "Interagir [F]", etc.
			int fontSize = 18; // Escolha um tamanho de fonte apropriado
			Color textColor = { 245, 245, 245, 240 }; // Um branco levemente acinzentado (WhiteSmoke), quase opaco

			// Calcula as dimensões do texto para posicionamento
			// Assumindo que existe MeasureText(const char *text, int fontSize)
			int textWidth = MeasureText( unlockText , fontSize );

			// Posiciona o texto: centralizado horizontalmente abaixo da porta, com algum espaçamento.
			int textPosX = static_cast< int >( doorCenter.x - textWidth / 2.0f );
			int textPosY = static_cast< int >( position.y + texture->height + 12 ); // 12 pixels abaixo da textura da porta

			// Opcional: Adicionar uma sombra sutil ou fundo ao texto para melhor legibilidade
			// DrawText(unlockText, textPosX + 1, textPosY + 1, fontSize, Fade(BLACK, 0.6f)); // Sombra simples

			// Desenha o texto principal
			DrawText( unlockText , textPosX , textPosY , fontSize , textColor );

			// Opcional: Adicionar um pequeno ícone de chave ao lado do texto ou da porta, se você tiver um.
			// Exemplo: if (texturaIconeChave) DrawTexture(*texturaIconeChave, textPosX - texturaIconeChave->width - 5, textPosY, WHITE);
		}
	}
}