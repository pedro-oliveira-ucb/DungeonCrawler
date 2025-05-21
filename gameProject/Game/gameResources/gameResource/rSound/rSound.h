#pragma once
#include <string>


class rSound
{
    std::string soundPath;
    void * soundAddress; // tipo opaco
    void initializeSound( );
public:
    rSound( const rSound & ) = delete;
    rSound & operator=( const rSound & ) = delete;

    rSound( std::string model );
    ~rSound( );

    void playSound( );
    void * getSound( ); // retorna tipo opaco
};
