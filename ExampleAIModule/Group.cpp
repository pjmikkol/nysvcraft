#include "Group.h"

using namespace std;

Group::Group()
{
	this->groupId = 0;
	this->units = new set<BWAPI::Unit*>();
	this->unitData = 0;
}

Group::~Group()
{
	delete this->units;
}

Group::Group(int id, map< BWAPI::Unit*, UnitData >* unitData)
{
	this->groupId = id;
	this->units = new set<BWAPI::Unit*>();
	this->unitData = unitData;
}

int Group::getId()
{
	return this->groupId;
}

void Group::add(BWAPI::Unit* unit)
{
	this->units->insert(unit);
}

void Group::remove(BWAPI::Unit* unit)
{
	set<BWAPI::Unit*>::iterator iter = this->units->find(unit);
	if (iter != this->units->end())
		this->units->erase(iter);
}

const set<BWAPI::Unit*>* Group::getUnits()
{
	return this->units;
}

int Group::getSize()
{
	return this->units->size();
}

void Group::setFormation(Formations f)
{
	this->form = Formation(f, this->units);
	this->form.makeFormation();
}
