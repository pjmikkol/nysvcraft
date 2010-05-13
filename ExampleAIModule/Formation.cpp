#include "Formation.h"
#include <cassert>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace helpers;
using namespace BWAPI;
using namespace std;


Formation::Formation(void)
{
	this->units = 0;
	this->form = free_will;
	this->labels = new map<Unit*, int>();
	this->isForming = false;
	this->leader = 0;
}

Formation::Formation(Formations f, std::set<BWAPI::Unit*>* units)
{
	this->units = units;
	this->form = f;
	switch(f) {
		case parabola:
			this->p1 = 0.2;
			this->distances = BWAPI::TILE_SIZE*30;
			this->angle = pi*3/2;
			break;
		default:
			this->distances = 0;
			this->p1 = this->angle = 0;
			break;
	}
	this->labels = new map<Unit*, int>();
	this->isForming = false;
	decideLeader();
}

void Formation::moveInFormation(Position to)
{
	double newangle = angleBetween(to, cos(this->angle), sin(this->angle));
	if (this->leader && this->leader->getHitPoints() > 0 && fabs(angle - newangle) > 0.01) {
		this->angle = newangle;
		decideLeader();
		// rotate the formation
	}
	else {
		
	}
}

void Formation::makeFormation() {
	printf("makeFormation()");
	if (!this->leader) decideLeader();
	bool inForm = this->inFormation();
	if (!isForming && !inForm) {
		Position center = this->leader->getPosition();
		foreach (Unit* unit, *this->units) {
			Position target = this->posInFormation(unit, center);
			unit->rightClick(target);
			Broodwar->printf("%d", (*this->labels)[unit]);
		}
		this->isForming = true;
	} else if (inForm) {
		this->isForming = false;
	}
}

void Formation::labelUnits()
{
	Broodwar->printf("labelUnits()");
	if (!this->leader) {
		Broodwar->printf("no leader in labelUnits()");	
		return;
	}
	double m = sin(this->angle)/cos(this->angle);
	Position p = this->leader->getPosition();
	double b = p.y() - m*p.x();
	int below = 0, above = 0;
	(*this->labels)[this->leader] = 0;
	foreach (Unit* unit, *this->units) {
		if (unit == this->leader) continue;
		Position pos = unit->getPosition();
		if (pos.y() < m*pos.x() + b) {
			below++; 
			(*this->labels)[unit] = below;
		} else {
			above++;
			(*this->labels)[unit] = -above;
		}
	}
}

void Formation::decideLeader()
{
	assert(this->units);
	assert(this->units->size() > 0);

	Broodwar->printf("decideLeader()");

	this->leader = 0;
	// TODO: ensure that there aren't dead units in units-set
	Position center = avgPos(units);
	// Using the form y = mx + b for line
	double m = static_cast<double>(sin(this->angle) - center.y())/(cos(this->angle) - center.x());
	foreach(Unit* i, *this->units) {
		Position iPos = i->getPosition();
		double b = iPos.y() - m*iPos.x();
		int below = 0, above = 0;
		foreach (Unit* j, *units) {
			if (i == j) continue;
			Position jPos = j->getPosition();
			if (jPos.y() < m*jPos.x() + b) below++;
			else above++;
		}
		if (abs(above - below) <= 1) {
			this->leader = i;
			labelUnits();
			return;
		}
	}
	this->leader = *(this->units->begin());
	labelUnits();

	assert(this->leader);
}

Position Formation::posInFormation(Unit* unit, Position center)
{
	assert(unit);
	int label = (*this->labels)[unit];
	int x = this->distances*label;
	//double xp = x;
	//double yp = this->p1*xp*xp;
	//rotCClockwise(xp, yp, this->angle);
	Position beforeRot(x, this->p1*x*x);
	Position p = rotCClockwise(beforeRot, this->angle);
	return Position(center.x() + p.x(), center.y() + p.y());
}

bool Formation::inFormation()
{
	if (this->form == free_will) return true;
	if (!this->leader) return false;
	Position center = this->leader->getPosition();
	foreach (Unit* i, *this->units) {
		Position target = posInFormation(i, center);
		if (target.getDistance( i->getPosition() ) > TILE_SIZE/2 ){
			return false;
		}
	}
	this->isForming = false;
	return true;
}