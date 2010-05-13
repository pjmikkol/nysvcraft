#pragma once
#include <BWAPI.h>
#include "Helpers.h"

enum Formations { free_will, parabola };

/* Base-class for representing the different formations */
class Formation
{
public:
	Formation(void);
	Formation(Formations f, std::set<BWAPI::Unit*>* units);
	void moveInFormation(BWAPI::Position to);
	BWAPI::Unit* decideLeader(std::set<BWAPI::Unit*>* units, BWAPI::Position to); //this one could belong to helpers
	bool inFormation();
	void labelUnits();
private:
	Formations form;
	std::map<BWAPI::Unit*, int> labels;
	// As a default the formation is directed along the vector (1,0) = (cos x, sin x), x = 0
	// here the angle is x
	// TODO: how the inverted y-axis will affect to the trig
	double angle;
	int distances; //Parameter which controls the distances between the units
	double p1; //first parameter to control formation 
	std::set<BWAPI::Unit*>* units;
	BWAPI::Unit* leader; // "Center" of the group. Other units move relatively to this
};
