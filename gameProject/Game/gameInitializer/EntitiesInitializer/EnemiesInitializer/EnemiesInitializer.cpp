#include "EnemiesInitializer.h"

#include <optional>

#include "../../../SDK/Entities/CEnemyEntity/CEnemyEntity.h"
#include "../../../SDK/Events/EventManager.h"
#include "../../../gameObjects/entitiesHandler/entitiesHandler.h"
#include "../../../gameObjects/gameSoundEventsHandler/gameSoundsEventHandler.h"

std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );

std::optional<CBaseEntityAnimationConstructor> createEntityAnimationConstructor(
	std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations
);

CEnemyEntity * EnemiesInitializer::generateEnemy( const std::string & animationName )
{
	// Exemplo de lista de animações para o inimigo
	std::vector<CBaseEntityAnimationType> requiredAnimations = {
		 CBaseEntityAnimationType::IDLE_FORWARD ,
		 CBaseEntityAnimationType::IDLE_BACKWARD ,
		 CBaseEntityAnimationType::IDLE_LEFT ,
		 CBaseEntityAnimationType::IDLE_RIGHT ,
		 CBaseEntityAnimationType::WALKING_FORWARD ,
		 CBaseEntityAnimationType::WALKING_BACKWARD ,
		 CBaseEntityAnimationType::WALKING_LEFT ,
		 CBaseEntityAnimationType::WALKING_RIGHT,
		 CBaseEntityAnimationType::ATTACKING_FORWARD ,
		 CBaseEntityAnimationType::ATTACKING_BACKWARD ,
		 CBaseEntityAnimationType::ATTACKING_LEFT ,
		 CBaseEntityAnimationType::ATTACKING_RIGHT,
		 CBaseEntityAnimationType::HURT_FORWARD ,
		 CBaseEntityAnimationType::HURT_BACKWARD ,
		 CBaseEntityAnimationType::HURT_LEFT ,
		 CBaseEntityAnimationType::HURT_RIGHT,
		 CBaseEntityAnimationType::DEAD_FORWARD ,
		 CBaseEntityAnimationType::DEAD_BACKWARD ,
		 CBaseEntityAnimationType::DEAD_LEFT ,
		 CBaseEntityAnimationType::DEAD_RIGHT
	};

	// Apenas exemplificado, reutilize a mesma lógica de criação utilizada em LocalPlayerInitializer.
	// Caso use 'createEntityAnimationConstructor', aplique o mesmo padrão:
	auto animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation ) { return nullptr; }

	CBaseEntityConstructor builder;
	builder.entityAnimations = animation.value( );
	builder.entityPosition = GVector2D( 0 , 0 );
	builder.entityType = CBaseEntityType::MOB;
	builder.health = 100;
	builder.movementSpeed = 1;
	builder.Name = animationName;

	// Cria a instância do inimigo
	return new CEnemyEntity( builder );
}

bool EnemiesInitializer::initializeEvents( std::string enemyName )
{
	// Exemplo de registro de eventos relacionados ao inimigo
	std::string eventName = enemyName + "_hurt";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ & ] ( ) {
			gameSoundsQueue.addEventToQueue( enemyName + "_hurt" );
		}
	) );

	eventName = enemyName + "_dead";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ & ] ( ) {
			gameSoundsQueue.addEventToQueue( enemyName + "_dead" );
		}
	) );

	eventName = enemyName + "_meleeAttack";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ & ] ( ) {
			gameSoundsQueue.addEventToQueue( enemyName + "_meleeAttack" );
		}
	) );

	eventName = enemyName + "_rangedAttack";
	EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		eventName ,
		[ & ] ( ) {
			gameSoundsQueue.addEventToQueue( enemyName + "_meleeAttack" );
		}
	) );

	return true;
}

bool EnemiesInitializer::initialize( )
{
	// Crie e registre um inimigo de exemplo (pode criar vários se quiser)
	std::vector<std::string> enemyNames = { "BasicEnemy", "AdvancedEnemy" };

	for ( const auto & enemyName : enemyNames ) {
		auto * enemy = generateEnemy( enemyName );
		if ( !enemy ) {
			return false;
		}
		// Adiciona ao EntitiesHandler
		entitiesHandler::Get( ).addEnemy( enemy );
		// Registra eventos
		if ( !initializeEvents( enemyName ) ) {
			return false;
		}
	}

	return true;
}