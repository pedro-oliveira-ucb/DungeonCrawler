#include "CRangedAttack.h"

#include "../../../../../Globals/Globals.h"
#include "../../../Utils/Log/Log.h"

CRangedAttack::CRangedAttack( CBaseEntityConstructor entityBuilder ,
	CBaseAttackConstructor attackBuilder
)
	:CBaseAttack( entityBuilder , attackBuilder )
{
}

void CRangedAttack::updateEntity( ) {
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

        bool animationFinished = this->getEntityAnimations( )->isAnimationFinished( );

        if ( positionDelta > this->getRange( ) || animationFinished ) {
            if ( positionDelta > this->getRange( ) ) {
                Log::Print( "[%s] Max attack range!" , this->GetEntityName( ).c_str( ) );
            }

            if ( animationFinished ) {
                Log::Print( "[%s] Attack animation finished!" , this->GetEntityName( ).c_str( ) );
                this->Deactive( );
            }
            return;
        }

        this->setEntityPosition( nextPosition );
        Log::Print( "[%s] Attack addPos: %f,%f, delta: %f, range: %f" ,
            this->GetEntityName( ).c_str( ) , newDirection.x , newDirection.y , positionDelta , this->getRange( ) );

        this->getEntityAnimations( )->updateAnimation( false );
    }
}

void CRangedAttack::otherDeactiveLogic( ) {

}

void CRangedAttack::otherActiveLogic( CBaseEntity * sender ) {

	float angleRad = this->getLookingAngle( ).getRadians( );
	float speed = this->getSpeed( );

	GVector2D senderSize = sender->getEntityAnimations( )->getCurrentTextureSize( );

	GVector2D newDirection(
		cosf( angleRad ) * senderSize.x ,
		sinf( angleRad ) * senderSize.y
	);

	GVector2D nextPosition = this->getEntityPosition( ) + newDirection;

	this->setEntityPosition( nextPosition );
	this->setInitialPosition( nextPosition );
}

std::shared_ptr<CBaseAttack> CRangedAttack::Clone( ) {
	std::shared_ptr<CRangedAttack> clone = std::make_shared<CRangedAttack>( *this ); // Usa cópia
	return clone;
}
