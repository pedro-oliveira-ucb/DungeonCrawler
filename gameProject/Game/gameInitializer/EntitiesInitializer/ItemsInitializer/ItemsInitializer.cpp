#include "ItemsInitializer.h"

#include <optional>

#include "../../../SDK/Events/EventManager.h"
#include "../../../SDK/Entities/Items/CHealthItem/CHealthItem.h"
#include "../../../SDK/Entities/Items/CKeyItem/CKeyItem.h"
#include "../../../SDK/Entities/Items/CManaItem/CManaItem.h"
#include "../../../Handlers/itemsHandler/itemsHandler.h"
#include "../../../Handlers/attackHandler/attackHandler.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"

#include "../../../../Utils/Log/Log.h"

std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );

std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations );

std::unique_ptr<CHealthItem> generateHealthItem( CBaseEntityConstructor & builder , const std::string & animationName , ItemType itemType , float healAmount )
{
	// Exemplo de lista de animações para o inimigo
	std::vector<CBaseEntityAnimationType> requiredAnimations = {
		 CBaseEntityAnimationType::IDLE_FORWARD ,
	};

	// Apenas exemplificado, reutilize a mesma lógica de criação utilizada em LocalPlayerInitializer.
	// Caso use 'createEntityAnimationConstructor', aplique o mesmo padrão:
	auto animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation ) {
		Log::Print( "[Item Initializer] %s animation generation failed!" , animationName.c_str( ) );
		return nullptr;
	}

	builder.entityAnimations = animation.value( );

	// Cria a instância do inimigo
	return std::make_unique<CHealthItem>( builder , itemType , healAmount );
}

template < typename T>
std::unique_ptr<T> generateItem( CBaseEntityConstructor & builder , const std::string & animationName , ItemType itemType ) {
	// Exemplo de lista de animações para o inimigo
	std::vector<CBaseEntityAnimationType> requiredAnimations = {
		 CBaseEntityAnimationType::IDLE_FORWARD ,
	};

	// Apenas exemplificado, reutilize a mesma lógica de criação utilizada em LocalPlayerInitializer.
	// Caso use 'createEntityAnimationConstructor', aplique o mesmo padrão:
	auto animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation ) {
		Log::Print( "[Item Initializer] %s animation generation failed!" , animationName.c_str( ) );
		return nullptr;
	}

	builder.entityAnimations = animation.value( );

	// Cria a instância do inimigo
	return std::make_unique<T>( builder , itemType  );
}

bool initializeEvents( std::string itemName )
{
	std::vector<std::string> eventsNames {
		"active",
		"effect",
	};

	for ( std::string event : eventsNames ) {
		std::string eventName = itemName + "_" + event;
		EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
			eventName ,
			[ eventName ] ( ) {
				gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
			}
		) );
	}

	return true;
}


bool ItemsInitializer::initialize( ) {

	CBaseEntityConstructor builder;
	builder.entityPosition = GVector2D( 0 , 0 );
	builder.entityType = CBaseEntityType::ITEM;
	builder.health = 100;
	builder.movementSpeed = 30;
	builder.entityHitbox = GVector2D( 20 , 20 );

	{
		builder.Name = "HealthItem";
		auto enemy = generateHealthItem( builder , builder.Name , ItemType::HEALTH_ITEM , 15 );
		if ( !enemy ) {
			Log::Print( "[Item Initializer] Failed to create %s!" , builder.Name.c_str( ) );
			return false;
		}

		if ( !initializeEvents( builder.Name ) ) {
			Log::Print( "[Item Initializer] Failed to events of %s!" , builder.Name.c_str( ) );
			return false;
		}
		itemsHandler::Get( ).addSpawnableItem( std::move( enemy ) );
	}


	{
		builder.Name = "KeyItem";
		std::unique_ptr< CKeyItem> keyItem = generateItem<CKeyItem>( builder , builder.Name , ItemType::KEY_ITEM );
		if ( !keyItem ) {
			Log::Print( "[Item Initializer] Failed to create %s!" , builder.Name.c_str( ) );
			return false;
		}

		if ( !initializeEvents( builder.Name ) ) {
			Log::Print( "[Item Initializer] Failed to events of %s!" , builder.Name.c_str( ) );
			return false;
		}
		itemsHandler::Get( ).addSpawnableItem( std::move( keyItem ) );
	}

	{
		builder.Name = "ManaItem";
		std::unique_ptr< CManaItem> manaItem = generateItem<CManaItem>( builder , builder.Name , ItemType::MANA_ITEM );
		if ( !manaItem ) {
			Log::Print( "[Item Initializer] Failed to create %s!" , builder.Name.c_str( ) );
			return false;
		}

		if ( !initializeEvents( builder.Name ) ) {
			Log::Print( "[Item Initializer] Failed to events of %s!" , builder.Name.c_str( ) );
			return false;
		}
		itemsHandler::Get( ).addSpawnableItem( std::move( manaItem ) );
	}


	return true;
}