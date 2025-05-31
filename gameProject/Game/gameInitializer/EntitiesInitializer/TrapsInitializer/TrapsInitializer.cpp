#include "TrapsInitializer.h"

#include <optional>

#include "../../../SDK/Events/EventManager.h"
#include "../../../Handlers/gameSoundEventsHandler/gameSoundsEventHandler.h"
#include "../../../Handlers/trapsHandler/trapsHandler.h"
#include "../../../SDK/Entities/Traps/CSpikeTrap/CSpikeTrap.h"

#include "../../../../Utils/Log/Log.h"

std::optional<std::pair<CBaseEntityAnimationType , rSpriteAnimation *>>
generateAnimationPair( CBaseEntityAnimationType type , std::string Name );


std::optional < CBaseEntityAnimationConstructor > createEntityAnimationConstructor( std::string animationName ,
	std::vector<CBaseEntityAnimationType> requiredAnimations );

std::unique_ptr<CSpikeTrap> generateSpikeTrap( CBaseEntityConstructor & builder , const std::string & animationName , TrapType trapType , float damageAmount )
{
	std::vector<CBaseEntityAnimationType> requiredAnimations = {
		 CBaseEntityAnimationType::IDLE_FORWARD ,
	};

	auto animation = createEntityAnimationConstructor( animationName , requiredAnimations );
	if ( !animation ) {
		Log::Print( "[Item Initializer] %s animation generation failed!" , animationName.c_str( ) );
		return nullptr;
	}

	builder.entityAnimations = animation.value( );

	// Cria a instância do inimigo
	return std::make_unique<CSpikeTrap>( builder , trapType , damageAmount );
}

bool initializeTrapEvents( std::string trapName )
{
	std::vector<std::string> eventsNames {
		"active",
		"effect",
	};

	for ( std::string event : eventsNames ) {
		std::string eventName = trapName + "_" + event;
		EventManager::Get( ).RegisterEvent( eventName , std::make_shared<CallbackEvent>(
			eventName ,
			[ eventName ] ( ) {
				gameSoundsEventHandler::Get( ).addEventToQueue( eventName );
			}
		) );
	}

	return true;
}

bool TrapsInitializer::initialize( )
{
	CBaseEntityConstructor builder;
	builder.entityPosition = GVector2D( 0 , 0 );
	builder.entityType = CBaseEntityType::TRAP;
	builder.health = 100;
	builder.movementSpeed = 30;
	builder.Name = "SpikeTrap";
	auto trap = generateSpikeTrap( builder , builder.Name , TrapType::SPIKES , 30 );
	if ( !trap ) {
		Log::Print( "[Item Initializer] Failed to create %s!" , builder.Name.c_str( ) );
		return false;
	}

	if ( !initializeTrapEvents( builder.Name ) ) {
		Log::Print( "[Item Initializer] Failed to events of %s!" , builder.Name.c_str( ) );
		return false;
	}

	trapsHandler::Get( ).addSpawnableTrap( std::move( trap ) );

    return true;
}
