#include "ExampleAIModule.h"
using namespace BWAPI;
void ExampleAIModule::onStart()
{
  Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
  // Enable some cheat flags
  Broodwar->enableFlag(Flag::UserInput);

  this->center = Position((Broodwar->mapWidth() * TILE_SIZE)/2 , (Broodwar->mapHeight() * TILE_SIZE)/2 );

  for(std::set<Unit*>::const_iterator i = Broodwar->self()->getUnits().begin();
	  i != Broodwar->self()->getUnits().end();
	  i++)
	(*i)->attackMove(this->center);  
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

	std::map< Unit*, int > * attacking = new std::map< Unit*, int >();

	for(std::set<Unit*>::const_iterator u = Broodwar->getAllUnits().begin();
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
				std::map<Unit*, int>::iterator iter = attacking->find(target);

				if (iter != attacking->end())
					(*iter).second++;
				else
					attacking->insert(std::make_pair(target,1));
			}
		}
	}

	for (std::map<Unit*, int>::const_iterator iter = attacking->begin(); iter != attacking->end(); iter++) {
		Position pos = iter->first->getPosition();
		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%d", iter->second);
	}

	delete attacking;
}

void ExampleAIModule::drawUnitInfo()
{
	for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
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
  }
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

bool ExampleAIModule::onSendText(std::string text)
{
  if (text=="/show players")
  {
    return false;
  }
  return true;
}