#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include "Formation.h"
#include "Helpers.h"

using namespace std;


/* Collection of units */
class Group
{
public:
	Group(int id, map< BWAPI::Unit*, UnitData >* unitData); // takes group id as a parameter
	void add(BWAPI::Unit*); // adds the unit to the group (if not already present)
	void remove(BWAPI::Unit*);
	const set<BWAPI::Unit*>* getUnits();
	int getSize();
	int getId();
	void setFormation(Formations f);

private:
	map< BWAPI::Unit*, UnitData >* unitData;
	int groupId; // Id of the group
	set<BWAPI::Unit*> units; // Pointers to units belonging to group
	Formation form; // The formation of group
	BWAPI::Unit* leader; // "Center" of the group. Other units move relatively to this
	BWAPI::Position target; // Target of the group
};