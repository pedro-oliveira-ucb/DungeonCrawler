#include "CMeleeAttack.h"

#include "../../../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

CMeleeAttack::CMeleeAttack( CBaseEntityConstructor entityBuilder ,
	CBaseAttackConstructor attackBuilder
)
	:CBaseAttack( entityBuilder , attackBuilder )
{
}

CMeleeAttack::CMeleeAttack( const CMeleeAttack & other ) 
	: CBaseAttack( other )
{
}

void CMeleeAttack::updateEntity( ) {
    if ( this->IsActive( ) ) {
      
        // Calcular deltaTime em segundos
        float deltaTime = 0.0f;
        if ( this->lastUpdateTime.time_since_epoch( ).count( ) > 0 ) {
            std::chrono::duration<float> delta = now - this->lastUpdateTime;
            deltaTime = delta.count( );
        }

        // Atualizar lastUpdateTime
        this->lastUpdateTime = now;

        float angleRad = this->getLookingAngle( ).getRadians( );
        float speed = this->getSpeed( ); // unidades por segundo
        GVector2D initialPos = this->getInitialPosition( );

        // Aplica deltaTime ao deslocamento
        GVector2D newDirection(
            std::cos( angleRad ) * speed * deltaTime ,
            std::sin( angleRad ) * speed * deltaTime
        );

        GVector2D nextPosition = this->getEntityPosition( ) + newDirection;
        float positionDelta = GVector2D( initialPos - nextPosition ).length( );

        this->getEntityAnimations( )->updateAnimation( false );

        bool animationFinished = this->getEntityAnimations( )->isAnimationFinished( );

        if ( positionDelta > this->getRange( ) || animationFinished ) {
            if ( animationFinished ) {
                //Log::Print( "[%s] Attack animation finished!" , this->GetEntityName( ).c_str( ) );
                this->Deactive( );
            }
            return;
        }

        this->setEntityPosition( nextPosition );
    }
}
void CMeleeAttack::otherActiveLogic( CBaseEntity * sender) {
	float angleRad = this->getLookingAngle( ).getRadians( );
	float speed = this->getSpeed( );

	GVector2D senderSize = sender->getEntityAnimations( )->getCurrentTextureSize( );

	GVector2D newDirection(
		cosf( angleRad ) * (senderSize.x),
		sinf( angleRad ) * (senderSize.y)
	);

	GVector2D nextPosition = this->getEntityPosition( ) + newDirection;

	this->setEntityPosition( nextPosition );
	this->setInitialPosition( nextPosition );
}

void CMeleeAttack::otherDeactiveLogic( ) {

}

std::shared_ptr<CBaseAttack> CMeleeAttack::Clone( ) {
	std::shared_ptr<CMeleeAttack> clone = std::make_shared<CMeleeAttack>( *this ); // Usa cópia
	return clone;
}
