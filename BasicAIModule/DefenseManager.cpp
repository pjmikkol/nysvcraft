#include <BWTA.h>
#include <DefenseManager.h>

DefenseManager::DefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator)
{
  this->arbitrator = arbitrator;
  std::set<BWTA::Chokepoint*> chokepoints = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion()->getChokepoints();
  if (chokepoints.size() == 1)
  {
    chokePosition = (*chokepoints.begin())->getCenter();
  }
  else
  {
    chokePosition = BWAPI::Positions::None;
  }
}

void DefenseManager::onOffer(std::set<BWAPI::Unit*> units)
{
  for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
  {
    if (defenders.find(*u) == defenders.end())
    {
      arbitrator->accept(this, *u);
      DefenseData temp;
      defenders.insert(std::make_pair(*u, temp));
    }
  }
}

void DefenseManager::onRevoke(BWAPI::Unit* unit, double bid)
{
  defenders.erase(unit);
}

void DefenseManager::onRemoveUnit(BWAPI::Unit* unit)
{
  defenders.erase(unit);
}

void DefenseManager::update()
{
  // Bid on all completed military units
  std::set<BWAPI::Unit*> myPlayerUnits=BWAPI::Broodwar->self()->getUnits();
  for (std::set<BWAPI::Unit*>::iterator u = myPlayerUnits.begin(); u != myPlayerUnits.end(); u++)
  {
    if ((*u)->isCompleted() && 
        !(*u)->getType().isWorker() && 
        !(*u)->getType().isBuilding() &&
        (*u)->getType() != BWAPI::UnitTypes::Zerg_Egg &&
        (*u)->getType() != BWAPI::UnitTypes::Zerg_Larva)
    {
      arbitrator->setBid(this, *u, 20);
    }
  }
  //Order all units to choke
  for (std::map<BWAPI::Unit*,DefenseData>::iterator u = defenders.begin(); u != defenders.end(); u++)
  {
    if ((*u).second.mode == DefenseData::Idle)
    {
      (*u).first->attackMove(chokePosition);
      (*u).second.mode = DefenseData::Moving;
    }
  }
}

std::string DefenseManager::getName() const
{
  return "Defense Manager";
}

std::string DefenseManager::getShortName() const
{
  return "Def";
}