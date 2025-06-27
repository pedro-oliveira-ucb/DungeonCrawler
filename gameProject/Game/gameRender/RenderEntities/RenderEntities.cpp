#include "RenderEntities.h"

#include "../../Managers/LevelManager/LevelManager.h"


#include "../../Handlers/attackHandler/attackHandler.h"
#include "../../Handlers/entitiesHandler/entitiesHandler.h"
#include "../../Handlers/itemsHandler/itemsHandler.h"
#include "../../Handlers/trapsHandler/trapsHandler.h"

#include "./../../SDK/math/math.h"

#include "../../../Utils/Log/Log.h"
#include "../../../Globals/Globals.h"

#include <raylib/raylib.h>

void renderEntity( CBaseEntity * entity , float sizeFactor = 1.0f ) {
	if ( entity == nullptr ) {
		return;
	}

	CBaseEntityAnimation * entityAnimation = entity->getEntityAnimations( );

	if ( entityAnimation == nullptr ) {
		return;
	}

	if ( entityAnimation->getCurrentTexture( ) == nullptr ) {
		Log::Print( "Texure of %s is null!" , entity->GetEntityName( ).c_str( ) );
		return;
	}

	Texture2D * texture = static_cast< Texture2D * >( entityAnimation->getCurrentTexture( ) );

	GVector2D hitbox = entity->getHitbox( ).getHitboxSize( );
	GVector2D size = entityAnimation->getCurrentTextureSize() * sizeFactor;
	GVector2D pos = entity->getEntityPosition( );

	Vector2 position = { pos.x, pos.y };
	Vector2 origin = { size.x / 2.0f, size.y / 2.0f };

	GAngle entityLookingAngle = entity->getLookingAngle( );
	GAngle entityMovingAngle = entity->getMovementAngle( );

	float rotationAngle = 0.0f;
	float baseAngle = entity->getEntityLookingDirectionBaseAngle( );
	float lookingAngleDeg = entity->getLookingAngle( ).getDegrees( );


	// calcula percentagem de vida [0..1]
	float maxH = float( entity->getMaxHealth( ) );
	float currH = float( entity->getHealth( ) );
	float ratio = ( maxH > 0 ) ? ( currH / maxH ) : 0.0f; // Evita divisão por zero

	// interpola R e G (B fica 0)
	unsigned char R = ( unsigned char ) ( ( 1.0f - ratio ) * 255.0f ); // vai de 0 -> 255
	unsigned char G = ( unsigned char ) ( ratio * 255.0f );         // vai de 255 -> 0
	Color healthColor = { R, G, 0, 255 };

	// --- CONFIGURAÇÕES DA NOVA BARRA DE VIDA ---

	// Dimensões e Posição
	float barWidth = hitbox.x * 0.8f; // 80% da largura da entidade
	float barHeight = 8.0f;         // Altura fixa, mais grossa
	float yOffset = size.y / 2.0f + 10.0f; // Distância acima da entidade

	// Posição do canto superior esquerdo da barra (centralizada acima da entidade)
	Vector2 barPosition = {
		pos.x - barWidth / 2.0f,
		pos.y - yOffset
	};

	// Cores
	Color bgColor = { 30, 30, 30, 200 }; // Fundo cinza escuro, semitransparente
	Color borderColor = { 0, 0, 0, 255 }; // Borda preta sólida

	// Cores para o gradiente da vida
	Color healthColorTop = { ( unsigned char ) ( R * 0.8f ), ( unsigned char ) ( G * 0.8f ), 0, 255 }; // Cor um pouco mais escura no topo
	Color healthColorBottom = { R, G, 0, 255 }; // Cor principal na base

	switch ( entity->getEntityType( ) ) {
	case CBaseEntityType::ATTACK:
		rotationAngle = lookingAngleDeg - 90;
		break;
	case CBaseEntityType::ITEM:	case CBaseEntityType::TRAP:
		rotationAngle = 0;
		break;
	case CBaseEntityType::MOB:

		// 1. Desenha o fundo da barra
		DrawRectangleV( barPosition , { barWidth, barHeight } , bgColor );

		// 2. Desenha a barra de vida (com gradiente)
		// Usamos DrawRectangleGradientV para o efeito.
		if ( ratio > 0 ) {
			DrawRectangleGradientEx( {
				barPosition.x ,
				barPosition.y ,
				barWidth * ratio,
				barHeight
				},		
				healthColorTop,          // Cor de baixo (no código, é a cor do topo do gradiente)
				healthColorBottom ,      // Cor de cima (no código, é a cor da base do gradiente)
				healthColorTop,          // Cor de baixo (no código, é a cor do topo do gradiente)
				healthColorBottom      // Cor de cima (no código, é a cor da base do gradiente)
			);
		}

		// 3. Desenha a borda ao redor da barra toda
		// Use DrawRectangleLinesEx para poder controlar a espessura da linha
		DrawRectangleLinesEx(
			{ barPosition.x, barPosition.y, barWidth, barHeight } ,
			1.5f , // Espessura da borda
			borderColor
		);

		rotationAngle = math::AngleDiff( lookingAngleDeg , baseAngle );
		break;
	default:
		rotationAngle = math::AngleDiff( lookingAngleDeg , baseAngle );
		break;
	}




	DrawTexturePro(
		*texture ,
		{ 0, 0, ( float ) texture->width, ( float ) texture->height } , // source = tamanho real da textura
		{ position.x, position.y, size.x, size.y } ,               // dest = tamanho desejado (escalado)
		origin ,
		rotationAngle ,
		WHITE
	);

#if _DEBUG 
	int offset = 0;

	DrawText( entity->GetEntityName( ).c_str( ) , pos.x - size.x / 2 , pos.y - size.y / 2 - 20 + offset , 10 , BLACK );

	offset -= 12;

	DrawText( CBaseEntityAnimation::getAnimationTypeName( entity->getEntityAnimations( )->getCurrentAnimationType( ) ).c_str( ) , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );

	offset -= 12;

	DrawText( std::to_string( entity->getEntityAnimations( )->getCurrentAnimationStep( ) ).c_str( ) , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );

	offset -= 12;

	DrawText( entity->getEntityStateAsString( ).c_str( ) , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );

	if ( entity->getEntityType( ) == CBaseEntityType::PLAYER ) {
		offset -= 12;
		float currentStamina = entity->getCurrentStamina( );

		DrawText( std::to_string( currentStamina ).c_str( ) , pos.x - size.x / 2.0f , pos.y - size.y / 2.0f - 20 + offset , 10 , BLACK );
	}



	DrawRectangleLines(
		pos.x - hitbox.x / 2 ,
		pos.y - hitbox.y / 2 ,
		hitbox.x ,
		hitbox.y ,
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
#endif
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

		renderEntity( attackPtr );
	}
}

void renderEnemies( ) {
	std::vector<std::unique_ptr<CEnemyEntity>> * enemies = entitiesHandler::Get( ).getSpawnedEnemies( );
	for ( int i = 0; i < enemies->size( ); i++ ) {
		CEnemyEntity * enemy = enemies->at( i ).get( );

		if ( enemy == nullptr )
			continue;

		if ( enemy->getEntityAnimations( ) == nullptr )
			continue;

		if ( !enemy->isAlive( ) && enemy->deathAnimationFinished( ) )
			continue;

		renderEntity( enemy );
	}
}

void renderItems( ) {

	std::vector<std::unique_ptr<CBaseItem>> * items = itemsHandler::Get( ).getSpawnedItems( );
	for ( int i = 0; i < items->size( ); i++ ) {
		CBaseItem * item = items->at( i ).get( );

		if ( item == nullptr )
			continue;

		if ( item->getEntityAnimations( ) == nullptr )
			continue;

		renderEntity( item );
	}
}

void renderTraps( ) {
	std::vector<std::unique_ptr<CBaseTrap>> * traps = trapsHandler::Get( ).getSpawnedTraps( );
	for ( int i = 0; i < traps->size( ); i++ ) {
		CBaseTrap * trap = traps->at( i ).get( );

		if ( trap == nullptr )
			continue;

		if ( trap->getEntityAnimations( ) == nullptr )
			continue;

		renderEntity( trap );
	}
}


void renderEntities::render( ) {
	renderTraps( );
	renderEntity( entitiesHandler::Get( ).getLocalPlayer( ) );
	renderAttacks( );
	renderEnemies( );
	renderItems( );
}