#pragma once
#include <BWAPI.h>

enum Formations { parabola };

/* Base-class for representing the different formations */
class Formation
{
public:
	Formation(Formations f, std::set<BWAPI::Unit*>* units);

private:

};
