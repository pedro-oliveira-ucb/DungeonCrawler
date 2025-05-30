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

bool utils::onProbability( int probability ) const {
	probability = std::clamp( probability , 0 , 100 );
	return randomNumber( 1 , 100 ) > ( 100 - probability );
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