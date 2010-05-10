#include "ExampleAIModule.h"
using namespace BWAPI;
void ExampleAIModule::onStart()
{
  Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
  // Enable some cheat flags
  Broodwar->enableFlag(Flag::UserInput);


  this->center = Position((Broodwar->mapWidth() * TILE_SIZE)/2 , (Broodwar->mapHeight() * TILE_SIZE)/2 );

  for(std::set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
  {
	(*i)->attackMove(this->center);
	Position pos = (*i)->getPosition();
	Broodwar->printf("Position: (%d,%d)", pos.x(), pos.y());
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