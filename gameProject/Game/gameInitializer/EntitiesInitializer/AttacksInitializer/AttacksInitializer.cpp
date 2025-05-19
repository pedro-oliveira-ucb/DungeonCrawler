#include "AttacksInitializer.h"
#include <optional>



std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );


std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations );


CBaseAttack * AttacksInitializer::generate( std::string animationName, CBaseAttackConstructor attackBuilder )
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

	return new CBaseAttack( builder, attackBuilder );
}