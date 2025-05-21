#include "AttacksInitializer.h"
#include <optional>

#include "../../../gameObjects/attackHandler/attackHandler.h"

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
	if ( !animation ) {
		return nullptr;
	}

	CBaseEntityConstructor builder;

	builder.entityAnimations = animation.value( );
	builder.entityPosition = GVector2D( 0 , 0 );
	builder.entityType = CBaseEntityType::ATTACK;
	builder.health = 100;
	builder.movementSpeed = 10;
	builder.Name = animationName;

	return new T( builder , attackBuilder );
}



bool AttacksInitializer::generateLocalPlayerAttacks( ) {

	CBaseAttackConstructor attackBuilder;
	{
		attackBuilder.damage = 10;
		attackBuilder.delay = .5f;
		attackBuilder.cooldown = 1.0f;
		attackBuilder.range = 5.f;
		attackBuilder.speed = 5;
		attackBuilder.type = CBaseAttackType_Melee;
		attackBuilder.Name = "MeleeAttack";
		//attack damage area
		attackBuilder.area = GVector2D( 5 , 5 );


		CMeleeAttack * raw = generateAttack<CMeleeAttack>( "localPlayerSimpleAttack" , attackBuilder );
		if ( raw == nullptr ) {
			Log::Print( "[generateLocalPlayerAttacks] Failed to generate meleeAttack" );
			return false;
		}
		std::shared_ptr<CMeleeAttack> newAttack( raw );

		attackHandler::Get( ).addAvailableLocalPlayerAttack( newAttack );
		Log::Print( "[EntitiesInitializer] Generated localPlayerSimpleAttack" );
	}
	 
	{
		attackBuilder.damage = 30;
		attackBuilder.delay = .5f;
		attackBuilder.cooldown = 1.0f;
		attackBuilder.range = 30.f;
		attackBuilder.speed = 10;
		attackBuilder.type = CBaseAttackType_Ranged;
		attackBuilder.Name = "MagicAttack";
		//attack damage area
		attackBuilder.area = GVector2D( 10 , 10 );

		CRangedAttack * raw = generateAttack<CRangedAttack>( "localPlayerHeavyAttack" , attackBuilder );
		if ( raw == nullptr ) {
			Log::Print( "[generateLocalPlayerAttacks] Failed to generate rangedAttack" );
			return false;
		}
		std::shared_ptr<CRangedAttack> newAttack( raw );

		attackHandler::Get( ).addAvailableLocalPlayerAttack( newAttack );

		//attackHandler::Get( ).addAvailableLocalPlayerAttack( std::make_shared<CBaseAttack>( newRangedAttack ) );
		Log::Print( "[EntitiesInitializer] Generated localPlayerHeavyAttack" );
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