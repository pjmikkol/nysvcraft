#include "Formation.h"
#include <cassert>
#include <algorithm>
#include <boost/bind.hpp>
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
	this->delay = 0;
}

Formation::Formation(Formations f, std::set<BWAPI::Unit*>* units)
{
	this->units = units;
	this->form = f;
	switch(f) {
		case parabola:
			this->p1 = 0.00;
			this->distances = 24;
			this->angle = pi*3/2;
			break;
		default:
			this->distances = 0;
			this->p1 = this->angle = 0;
			break;
	}
	this->labels = new map<Unit*, int>();
	this->isForming = false;
	this->delay = 0;
	decideLeader();
}

void Formation::moveInFormation(Position to)
{
	if (this->delay-- > 0) return;
	this->delay = 15;
	double newangle = angleBetween(to, cos(this->angle), sin(this->angle));
	if (!this->leader || this->leader->getHitPoints() <= 0)
		decideLeader();
	if (fabs(this->angle - newangle) > 0.1) {
		Broodwar->printf("newangle %f %f", this->angle, newangle);
		this->angle = newangle;
		//decideLeader();
		this->isForming = true;
		Position dir = vecFromAngle(this->angle, 1);
		this->leader->rightClick(this->leader->getPosition() + Position(dir.x()/2, dir.y()/2));
		makeFormation();
	}
	else if (!inFormation()) {
		Broodwar->printf("not in Formation");
		this->isForming = true;
		//this->leader->rightClick(this->leader->getPosition() + vecFromAngle(this->angle, 1));
		makeFormation();
	}
	else {
		Broodwar->printf("move to pos");
		foreach(Unit* unit, *this->units)
			unit->rightClick(unit->getPosition() + vecFromAngle(this->angle, 3));
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
			Broodwar->printf("%d, %d", target.x(), target.y());
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
	// Again we use form y = mx + b for line 
	double m = sin(this->angle)/cos(this->angle);
	Position p = this->leader->getPosition();
	double b = p.y() - m*p.x();

	vector<Unit*> bel = vector<Unit*>();
	vector<Unit*> ab = vector<Unit*>();
	int below = 0, above = 0;
	(*this->labels)[this->leader] = 0;
	foreach (Unit* unit, *this->units) {
		if (unit == this->leader) continue;
		Position pos = unit->getPosition();
		if (pos.y() <= m*pos.x() + b) {
			bel.push_back(unit);
		} else {
			ab.push_back(unit);
		}
	}
	while (bel.size() >= ab.size() + 1 && ab.size() > 0) {
		ab.push_back(bel[bel.size() - 1]);
		bel.pop_back();
	}
	while (ab.size() >= bel.size() + 1 && bel.size() > 0) {
		bel.push_back(ab[ab.size() - 1]);
		ab.pop_back();
	}

	sort(ab.begin(), ab.end(), bind(cmpUnit,this->leader,_1,_2));
	sort(bel.begin(), bel.end(), bind(cmpUnit,this->leader,_1,_2));
	for(int i = 0; i < ab.size(); i++)
		(*this->labels)[ab[i]] = -(i+1);
	for(int i = 0; i < bel.size(); i++)
		(*this->labels)[bel[i]] = i+1;
}

bool cmpUnit(Unit *c, Unit* u, Unit* v)
{
	Position cen = c->getPosition();
	if ( u->getPosition().getDistance(cen) < v->getPosition().getDistance(cen) )
		return true;
	else return false;
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
	Unit* maybeLeader = 0;
	foreach(Unit* i, *this->units) {
		Position iPos = i->getPosition();
		double b = iPos.y() - m*iPos.x();
		int below = 0, above = 0;
		foreach (Unit* j, *units) {
			if (i == j) {
				maybeLeader = i;
				continue;
			}
			Position jPos = j->getPosition();
			if (jPos.y() < m*jPos.x() + b) below++;
			else above++;
		}
		int diff = abs(above - below);
		if (diff == 0) {
			this->leader = i;
			labelUnits();
			return;
		}
		else if (diff <= 1) {
			maybeLeader = i;
		}
	}
	this->leader = maybeLeader;
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