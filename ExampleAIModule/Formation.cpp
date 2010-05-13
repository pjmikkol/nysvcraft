#include "Formation.h"
#include <cassert>

using namespace helpers;
using namespace BWAPI;
using namespace std;

Formation::Formation()
{
	this->distances = 0;
	this->p1 = this->angle = 0;
	this->form = free_will;
	this->units = 0;
	this->leader = 0;
	this->labels = map<Unit*, int>();
}

Formation::Formation(Formations f, std::set<BWAPI::Unit*>* units)
{
	this->units = units;
	this->form = f;
	switch(f) {
		case parabola:
			this->p1 = 0.002;
			this->distances = BWAPI::TILE_SIZE;
			this->angle = 0;
		default:
			this->distances = 0;
			this->p1 = this->angle = 0;
	}
	this->leader = 0;
	this->labels = map<Unit*, int>();
}

void Formation::moveInFormation(Position to)
{
	double newangle = angleBetween(to, cos(this->angle), sin(this->angle));
	if (this->leader && this->leader->getHitPoints() > 0 && fabs(angle - newangle) > 0.01) {
		this->angle = newangle;
		this->leader = decideLeader(this->units, to);
		// rotate the formation	
	}
	else {
		
	}
}

void Formation::labelUnits()
{
	if (!this->leader) return;
	double m = sin(this->angle)/cos(this->angle);
	Position p = this->leader->getPosition();
	double b = p.y() - m*p.x();
	int below = 0, above = 0;
	this->labels.insert(make_pair(this->leader, 0));
	for(set<Unit*>::const_iterator i = this->units->begin(); i != this->units->end(); i++) {
		if (*i == this->leader) continue;
		Position iPos = (*i)->getPosition();
		if (iPos.y() < m*iPos.x() + b) {
			below++; 
			this->labels[*i] = below;
		} else {
			above++;
			this->labels[*i] = -above;
		}
	}
}


// This one could be in helpers also
Unit* Formation::decideLeader(set<Unit*>* units, Position to)
{
	// TODO: ensure that there aren't dead units in units-set
	Position center = avgPos(units);
	// Using the form y = mx + b for line
	double m = static_cast<double>(to.y() - center.y())/(to.x() - center.x());
	for(set<Unit*>::const_iterator i = units->begin(); i != units->end(); i++) {
		Position iPos = (*i)->getPosition();
		double b = iPos.y() - m*iPos.x();
		int below = 0, above = 0;
		for(set<Unit*>::const_iterator j = units->begin(); j != units->end(); j++) {
			if (*i == *j) continue;
			Position jPos = (*j)->getPosition();
			if (jPos.y() < m*jPos.x() + b) below++;
			else above++;
		}
		if (abs(above - below) <= 1) {
			return (*i);
		}
	}
	//Control should never reach here.
	assert(false); 
	return 0;
}

Position Formation::posInFormation(Unit* unit, Position center)
{
	int label = this->labels[unit];
	int x = this->distances*label;
	Position beforeRot(x, (int) (this->p1*x*x));
	return center + rotCClockwise(beforeRot,this->angle);
}

bool Formation::inFormation()
{
	if (this->form == free_will) return true;

	Position center = this->leader->getPosition();
	for(set<Unit*>::const_iterator i = units->begin(); i != units->end(); i++) {
		Position target = posInFormation(*i, center);
		if (target.getDistance( (*i)->getPosition() ) > TILE_SIZE/2 )
			return false;
	}
	return true;
}