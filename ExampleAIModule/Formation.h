#pragma once
#include <BWAPI.h>

enum Formations { free_will, parabola };

/* Base-class for representing the different formations */
class Formation
{
public:
	Formation(void);
	Formation(Formations f, std::set<BWAPI::Unit*>* units);
	void moveInFormation(BWAPI::Position to);

private:
	Formations form;
	// As a default the formation is directed along the vector (1,0) = (cos x, sin x), x = 0
	// here the angle is x
	// TODO: how the inverted y-axis will affect to the trig
	double angle;
	double distances; //Parameter which controls the distances between the units
	double p1; //first parameter to control formation

	std::set<BWAPI::Unit*>* units;
};
