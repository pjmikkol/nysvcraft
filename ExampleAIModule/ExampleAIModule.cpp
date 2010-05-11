#include "ExampleAIModule.h"

using namespace BWAPI;
using namespace std;

string stateName(State state) {
	switch (state) {
		case flee:
			return "flee";
		case fight:
			return "fight";
		case find_enemy:
			return "find_enemy";
	}
}

void ExampleAIModule::onStart()
{
	Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
	Broodwar->enableFlag(Flag::UserInput);

	this->center = Position((Broodwar->mapWidth() * TILE_SIZE)/2 , (Broodwar->mapHeight() * TILE_SIZE)/2 );
	this->unitData = map< Unit*, UnitData >();

	for(set<Unit*>::const_iterator i = Broodwar->self()->getUnits().begin();
	    i != Broodwar->self()->getUnits().end();
	    i++) {
		
		Unit* unit = *i;
		unit->attackMove(this->center);  
		UnitData unitData;
		unitData.state = find_enemy;
		this->unitData.insert(make_pair(unit, unitData));
		
		Broodwar->printf("Initial hit points: %d", unit->getType().maxHitPoints());
	}
}

void ExampleAIModule::onEnd(bool isWinner)
{
  if (isWinner)
  {
    //log win to file
  }
}

void ExampleAIModule::onFrame()
{
	drawUnitInfo();

	map< Unit*, int > * attacking = this->getAttackerCount();
	
	for (map<Unit*, int>::const_iterator iter = attacking->begin(); iter != attacking->end(); iter++) {
		Position pos = iter->first->getPosition();
		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%d", iter->second);
	}

	for (map<Unit*, int>::const_iterator iter = attacking->begin(); iter != attacking->end(); iter++) {		
		Unit* unit = iter->first;
		int attackers = iter->second;		

		map<Unit*, UnitData>::iterator dataIter = this->unitData.find(unit);

		UnitData data = dataIter->second;

		if (data.state == flee)
			continue;

		if (5 * unit->getHitPoints() < unit->getType().maxHitPoints()) {
			TilePosition current = unit->getTilePosition();			
			TilePosition runTo = current - TilePosition(5, 5);
			
			data.state = flee;

			this->unitData.insert(make_pair(unit, data));

			unit->rightClick(runTo);
		}			
	}

	delete attacking;
}

map< Unit*, int > * ExampleAIModule::getAttackerCount() {
	map< Unit*, int > * attacking = new map< Unit*, int >();

	for(set<Unit*>::const_iterator u = Broodwar->getAllUnits().begin();
		u != Broodwar->getAllUnits().end();
		u++)
	{
		Unit* unit = *u;

		if (unit->getPlayer() == Broodwar->self())
			continue;
			
		Unit* target = unit->getOrderTarget();

		if (target) {
			double distance = unit->getDistance(target);

			Broodwar->drawTextMap(unit->getPosition().x() - 16, unit->getPosition().y() - 26, "%f", distance);

			if (distance <= unit->getType().groundWeapon()->maxRange()) {
				map<Unit*, int>::iterator iter = attacking->find(target);

				if (iter != attacking->end())
					(*iter).second++;
				else
					attacking->insert(make_pair(target,1));
			}
		}
	}

	return attacking;
}

void ExampleAIModule::drawUnitInfo()
{
	for(set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		Position pos = (*i)->getPosition();
		Unit* targetUnit = (*i)->getOrderTarget();
		Position target = (*i)->getTargetPosition();
		Color color;

		if (targetUnit)
			color = Color(Colors::Red);
		else
			color = Color(Colors::Green);
			
		Broodwar->drawLineMap(pos.x(), pos.y(), target.x(), target.y(), color);

		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 16, "\x05(%d, %d)", pos.x(), pos.y());

		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%s", stateName(this->getUnitData(*i).state).c_str());
  }
}

UnitData ExampleAIModule::getUnitData(Unit* unit) {
	return this->unitData.find(unit)->second;
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitMorph(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitShow(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitHide(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onPlayerLeft(BWAPI::Player* player)
{

}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{

}

bool ExampleAIModule::onSendText(string text)
{
  if (text=="/show players")
  {
    return false;
  }
  return true;
}