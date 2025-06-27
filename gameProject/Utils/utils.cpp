#include <Windows.h>

#include "utils.h"
#include <random>
#include <sstream>
#include <fstream>


std::random_device rd; // Pode ser um membro da classe
std::mt19937 rng;      // Pode ser um membro da classe


int utils::randomNumber( int min , int max ) const
{
	std::uniform_int_distribution<int> uni( min , max );
	return uni( rng );
}

utils::utils( )
{
	rng = std::mt19937( rd( ) );
}

bool utils::onProbability( int probability ) const {
	probability = std::clamp( probability , 0 , 100 );
	return randomNumber( 1 , 100 ) > ( 100 - probability );
}

std::pair<int , int> utils::getScreenSize( ) const {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow( );
	GetWindowRect( hDesktop , &desktop );
	return std::make_pair( desktop.right , desktop.bottom );
}

void utils::freeConsole( )
{
	FreeConsole( );
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