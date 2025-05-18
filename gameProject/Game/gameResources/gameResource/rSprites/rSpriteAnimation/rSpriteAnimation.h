#pragma once
#include <string>
#include <vector>
#include <memory>

#include "../rSprite/rSprite.h"

enum SPRITE_TYPE {
    STYLESHEET ,
    SINGULAR
};

class rSpriteAnimation
{
	std::vector<std::unique_ptr<rSprite>> sprites;
	std::string modelName;
	std::string modelFolderPath;
	int currentAnimationStep = 0;
	SPRITE_TYPE _spriteType;

	void initializeAnimation( );
public:
	rSpriteAnimation( std::string model, std::string path) : modelName( model ), modelFolderPath(path)
	{ initializeAnimation( ); }

	void * getCurrentTexture( );
	GVector2D getCurrentSpriteSize( );
	std::string getModelName( );
	std::string getModelPath( );
	void updateAnimation(bool loop = true, bool reverse= false);
};

