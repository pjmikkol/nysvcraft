#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <limits>

namespace helpers {
	BWAPI::Unit* getClosestUnitFrom(BWAPI::Position &pos, std::set<BWAPI::Unit*> units);
	BWAPI::Unit* getClosestEnemy(BWAPI::Unit* unit, std::set<BWAPI::Unit*> enemies);
	std::set<BWAPI::Unit*> getAttackingAllies();
	bool isAttackingEnemy(BWAPI::Unit* unit);
}