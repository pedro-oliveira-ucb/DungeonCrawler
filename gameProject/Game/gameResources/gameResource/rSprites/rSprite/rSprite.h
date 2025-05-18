#pragma once
#include <string>

class rSprite
{
    std::string modelPath;
    void * texture; // tipo opaco

    void initializeTexture( );

public:
    rSprite( const rSprite & ) = delete;
    rSprite & operator=( const rSprite & ) = delete;

    rSprite( std::string model );
    ~rSprite( );

    void * getTexture( ); // retorna tipo opaco
};
