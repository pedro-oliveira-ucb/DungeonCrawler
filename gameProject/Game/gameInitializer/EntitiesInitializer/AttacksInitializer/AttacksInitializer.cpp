#include "AttacksInitializer.h"
#include <optional>

#include "../../../gameObjects/attackHandler/attackHandler.h"
#include "../../../gameObjects/gameSoundEventsHandler/gameSoundsEventHandler.h"

#include "../../../SDK/Events/EventManager.h"

#include "../../../../Utils/Log/Log.h"



std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );


std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations );


template <typename T>
T * AttacksInitializer::generateAttack( std::string animationName , CBaseAttackConstructor attackBuilder )
{
	std::vector<CBaseEntityAnimationType> requiredAnimations = {
			CBaseEntityAnimationType::ATTACKING_FORWARD ,
	};

	std::optional<CBaseEntityAnimationConstructor> animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation.has_value() ) {
		return nullptr;
	}

	animation.value( ).currentAnimationType = ATTACKING_FORWARD;

	CBaseEntityConstructor builder;

	builder.entityAnimations = animation.value( );
	builder.entityPosition = GVector2D( 0 , 0 );
	builder.entityType = CBaseEntityType::ATTACK;
	builder.health = 100;
	builder.movementSpeed = 10;
	builder.Name = animationName;

	T * result = new T( builder , attackBuilder );

	return result;
}



bool AttacksInitializer::generateLocalPlayerAttacks( ) {

	CBaseAttackConstructor attackBuilder;
	{
		//attackBuilder.damage = 10;
		//attackBuilder.delay = .5f;
		//attackBuilder.cooldown = 5.0f;
		//attackBuilder.range = 5.f;
		//attackBuilder.speed = 5;
		//attackBuilder.type = CBaseAttackType_Melee;
		//attackBuilder.Name = "MeleeAttack";
		////attack damage area
		//attackBuilder.area = GVector2D( 5 , 5 );


		//CMeleeAttack * raw = generateAttack<CMeleeAttack>( "localPlayerSimpleAttack" , attackBuilder );
		//if ( raw == nullptr ) {
		//	Log::Print( "[generateLocalPlayerAttacks] Failed to generate meleeAttack" );
		//	return false;
		//}
		//std::shared_ptr<CMeleeAttack> newAttack( raw );

		//attackHandler::Get( ).addAvailableLocalPlayerAttack( newAttack );
		//Log::Print( "[EntitiesInitializer] Generated localPlayerSimpleAttack" );

		//std::string eventName = "localPlayerSimpleAttack_attackThrow";
		//EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
		//	eventName ,
		//	[ ] ( ) {

		//		gameSoundsQueue.addEventToQueue( "localPlayerSimpleAttack_attackThrow" );
		//	}
		//) );
	}
	 
	{
		//attackBuilder.damage = 50;
		//attackBuilder.delay = .5f;
		//attackBuilder.cooldown = 30.0f;
		//attackBuilder.range = 1000.f;
		//attackBuilder.speed = 50;
		//attackBuilder.type = CBaseAttackType_Ranged;
		//attackBuilder.Name = "MagicAttack";
		////attack damage area
		//attackBuilder.area = GVector2D( 10 , 10 );

		//CRangedAttack * raw = generateAttack<CRangedAttack>( "localPlayerHeavyAttack" , attackBuilder );
		//if ( raw == nullptr ) {
		//	Log::Print( "[generateLocalPlayerAttacks] Failed to generate rangedAttack" );
		//	return false;
		//}
		//std::shared_ptr<CRangedAttack> newAttack( raw );

		//attackHandler::Get( ).addAvailableLocalPlayerAttack( newAttack );

		////attackHandler::Get( ).addAvailableLocalPlayerAttack( std::make_shared<CBaseAttack>( newRangedAttack ) );
		//Log::Print( "[EntitiesInitializer] Generated localPlayerHeavyAttack" );
	}

	return true;
}

bool AttacksInitializer::initializeAttacks( ) {

	if ( !generateLocalPlayerAttacks( ) ) { 
		Log::Print( "[initializeAttacks] Failed to generate localPlayerAttacks" );
		return false;
	} 


	return true;
}