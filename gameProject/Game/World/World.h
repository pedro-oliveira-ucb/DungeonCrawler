#pragma once

class World
{
public:
	void * gameWorldPointer = nullptr;
	void * resourceManagerPointer = nullptr;
};

extern World world;

