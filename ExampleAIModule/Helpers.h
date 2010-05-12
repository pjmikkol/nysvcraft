#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <limits>
#include <cmath>

/*States are:
	flee			- for running away from enemy
	fight			- kicking ass
	formation		- when moving in formation finding enemy or doing something
*/
enum State { flee, fight, formation };

struct UnitData {
	State state;
	int fleeCounter;
	int group; // Which group the unit belongs?
	int attackCounter;
};

namespace helpers {
	BWAPI::Unit* getClosestUnitFrom(BWAPI::Position &pos, std::set<BWAPI::Unit*> units);
	BWAPI::Unit* getClosestEnemy(BWAPI::Unit* unit, std::set<BWAPI::Unit*> enemies);
	std::set<BWAPI::Unit*> getAttackingAllies();
	bool isAttackingEnemy(BWAPI::Unit* unit);

	// Returns the angle between two vectors
	// Interprets given positions as origo-centered vectors
	double angleBetween(BWAPI::Position, BWAPI::Position);
}