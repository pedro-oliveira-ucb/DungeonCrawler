#include "CMeleeAttack.h"


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

void CMeleeAttack::updateAttackPosition( ) {
	if ( this->IsActive( ) ) {

		float angleRad = this->getLookingAngle( ).getRadians( );
		float speed = this->getSpeed( );
		GVector2D initialPos = this->getInitialPosition( );

		GVector2D newDirection(
			cosf( angleRad ) * speed ,
			sinf( angleRad ) * speed
		);

		GVector2D nextPosition = this->getEntityPosition( ) + newDirection;

		float positionDelta = GVector2D( initialPos - nextPosition ).length( );

		bool animationFinished = this->getEntityAnimations( )->getAnimationCycle( );

		if ( positionDelta > this->getRange( ) || animationFinished ) {
			if ( positionDelta > this->getRange( ) )
			{
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
