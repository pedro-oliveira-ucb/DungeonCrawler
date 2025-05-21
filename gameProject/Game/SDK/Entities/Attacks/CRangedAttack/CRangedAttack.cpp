#include "CRangedAttack.h"

#include "../../../Utils/Log/Log.h"

CRangedAttack::CRangedAttack( CBaseEntityConstructor entityBuilder ,
	CBaseAttackConstructor attackBuilder
)
	:CBaseAttack( entityBuilder , attackBuilder )
{
}

void CRangedAttack::updateAttackPosition( ) {
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

		if ( positionDelta > this->getRange() || animationFinished ) {
			Log::Print( "[%s] Attack out of range" , this->GetEntityName( ).c_str( ) );
			if ( animationFinished ) {
				this->Deactive();
			}
			return;
		}

		this->setEntityPosition( nextPosition );
		Log::Print( "[%s] Attack addPos: %f,%f, delta: %f, range: %f" ,
			this->GetEntityName( ).c_str( ) , newDirection.x , newDirection.y , positionDelta , this->getRange() );
	}
}

void CRangedAttack::otherDeactiveLogic( ) {

}

void CRangedAttack::otherActiveLogic( CBaseEntity * sender ) {

	float angleRad = this->getLookingAngle( ).getRadians( );
	float speed = this->getSpeed( );

	GVector2D senderSize = sender->getEntityAnimations( )->getCurrentTextureSize( );


	GVector2D newDirection(
		cosf( angleRad ) * speed + senderSize.x,
		sinf( angleRad ) * speed + senderSize.y
	);

	GVector2D nextPosition = this->getEntityPosition( ) + newDirection;

	this->setEntityPosition( nextPosition );
	this->setInitialPosition( nextPosition );

}

std::shared_ptr<CBaseAttack> CRangedAttack::Clone( ) {
	std::shared_ptr<CRangedAttack> clone = std::make_shared<CRangedAttack>( *this ); // Usa cópia
	return clone;
}
