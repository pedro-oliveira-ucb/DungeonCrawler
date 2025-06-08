#pragma once
#include <string>
#include "../../gameResources/gameResource/rSprites/rSprite/rSprite.h"	

#include <memory>

enum class mapObjectType
{
	mapObjectNone,
	tallWall, //64x64
	rightlateralTallWall, //64x64
	leftlateralTallWall ,  // 64x64
	ground, //16x16
	door, //64x64
	unlocked_door,//64x64
	mapObjectCount
};

enum mapType {
	firstMap ,
	secondMap ,
	thirdMap ,
	bossMap
};

class rMapObject
{
	mapObjectType objectType;
	std::shared_ptr<rSprite> sprite;
public:
	rMapObject(mapObjectType type, std::shared_ptr<rSprite> spritePtr)
		: objectType( type ) , sprite( spritePtr ) {
	}

	mapObjectType getObjectType() const {
		return objectType;
	}

	std::shared_ptr<rSprite> * getSprite( ) {
		return &this->sprite;
	}

	static std::string geMapObjectTypeName( mapObjectType type ) {
		switch ( type ) {
		case mapObjectType::tallWall:
			return "tallWall";
		case mapObjectType::leftlateralTallWall:
			return "leftlateralTallWall";
		case mapObjectType::rightlateralTallWall:
			return "rightlateralTallWall";
		case mapObjectType::ground:
			return "ground";
		case mapObjectType::door:
			return "door";
		case mapObjectType::unlocked_door:
			return "unlocked_door";
		default:
			return "unknown";
		}
	}	
};

