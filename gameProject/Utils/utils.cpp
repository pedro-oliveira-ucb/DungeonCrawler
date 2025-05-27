#include "utils.h"
#include <random>
#include <sstream>
#include <fstream>

int utils::randomNumber( int min , int max ) const
{
	std::random_device rd;
	std::mt19937 rng( rd( ) );
	std::uniform_int_distribution<int> uni( min , max );
	return uni( rng );
}

std::string utils::readFileAsString( const std::string & filename ) const

{
	std::ifstream file( filename );
	if ( !file.is_open( ) ) {
		return ""; // Or throw an exception
	} 
	std::stringstream buffer;
	buffer << file.rdbuf( );
	return buffer.str( );
}