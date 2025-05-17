#include "rBaseResource.h"

rBaseResource::rBaseResource( std::string path) {
	this->Path = path;
}

std::string rBaseResource::getPath( ) {
	return this->Path;
}