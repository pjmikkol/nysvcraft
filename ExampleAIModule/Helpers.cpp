#include "Helpers.h"

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace BWAPI;
using namespace std;

namespace helpers {
	Unit* getClosestEnemy(Unit* unit, set<Unit*> enemies) {		
		return getClosestUnitFrom(unit->getPosition(), enemies);
	}

	Unit* getClosestUnitFrom(Position &pos, set<Unit*> units) {
		Unit* closest = 0;
		double minDistance = numeric_limits<double>::infinity();

		foreach (Unit* unit, units) {
			double distance = unit->getPosition().getDistance(pos);

			if (distance < minDistance) {
				minDistance = distance;
				closest = unit;
			}
		}

		return closest;
	}

	bool isAttackingEnemy(Unit* unit) {
		Unit* other = unit->getOrderTarget();
		return other && other->getPlayer() == Broodwar->enemy();
	}

	set<Unit*> getAttackingAllies() {
		set<Unit*> allies = Broodwar->self()->getUnits();
		
		foreach (Unit* ally, allies) {
			if (!isAttackingEnemy(ally))
				allies.erase(ally);	
		}

		return allies;
	}

	double angleBetween(Position p1, Position p2)
	{
		double len1 = p1.getLength();
		double len2 = p2.getLength();
		double p = (p1.x()*p2.x() + p1.y()*p2.y())/(len1*len2);
		return acos(p);
	}

	double angleBetween(Position p, double x, double y)
	{
		double len = sqrt(x*x + y*y);
		double v = (p.x()*x + p.y()*y)/(len*p.getLength());
		return acos(v);
	}

	Position vecFromAngle(double angle, int tiles)
	{
		int coef = TILE_SIZE*tiles;
		return Position(static_cast<int>(cos(angle)*coef), static_cast<int>(sin(angle)*coef));
	}

	int killedUnitCount() {
		int count = 0;

		for (int i = 0; i < typeCount; i++) {
			count += Broodwar->self()->killedUnitCount(UnitTypes::getUnitType(string(unitTypes[i])));
		}

		return count;
	}

	int deadUnitCount() {
		int count = 0;

		for (int i = 0; i < typeCount; i++) {
			count += Broodwar->self()->deadUnitCount(UnitTypes::getUnitType(string(unitTypes[i])));
		}

		return count;
	}
}

BWAPI::Position helpers::avgPos(std::set<BWAPI::Unit*>* units) 
{
	BWAPI::Position p(0,0);
	for(std::set<Unit*>::const_iterator i = units->begin(); i != units->end(); i++)
		p += (*i)->getPosition();
	return BWAPI::Position(p.x()/units->size(), p.y()/units->size());
}

Position helpers::rotClockwise(Position p, double angle)
{
	double c = cos(angle), s = sin(angle);
	return Position((int) (p.x()*c + s*p.y()), (int) (c*p.y() - s*p.x()));
}

Position helpers::rotCClockwise(Position p, double angle)
{
	double c = cos(angle), s = sin(angle);
	return Position((int) (p.x()*c - s*p.y()), (int) (s*p.x() + c*p.y()));
}