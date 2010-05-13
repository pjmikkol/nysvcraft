#pragma once
#include <BWAPI.h>
#include "Helpers.h"

enum Formations { free_will, parabola };

bool cmpUnit(BWAPI::Unit* c,BWAPI::Unit* u, BWAPI::Unit* v);

/* Base-class for representing the different formations */
class Formation
{
public:
	Formation(void);
	Formation(Formations f, std::set<BWAPI::Unit*>* units);
	void moveInFormation(BWAPI::Position to);
	void decideLeader();
	bool inFormation();
	void labelUnits();
	BWAPI::Position posInFormation(BWAPI::Unit* unit, BWAPI::Position center);
	void makeFormation();
private:
	Formations form;
	std::map<BWAPI::Unit*, int>* labels;
	// As a default the formation is directed along the vector (1,0) = (cos x, sin x), x = 0
	// here the angle is x
	// TODO: how the inverted y-axis will affect to the trig
	double angle;
	int distances; //Parameter which controls the distances between the units
	double p1; //first parameter to control formation 
	bool isForming;
	std::set<BWAPI::Unit*>* units;
	BWAPI::Unit* leader; // "Center" of the group. Other units move relatively to this
};
