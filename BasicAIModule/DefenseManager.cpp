#include <BWTA.h>
#include <DefenseManager.h>
#include <cassert>

using namespace BWAPI;
using namespace std;
using namespace BWTA;

DefenseManager::DefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator, BuildOrderManager* buildOrderManager, BaseManager* baseManager)
{
	this->arbitrator = arbitrator;
	this->buildOrderManager = buildOrderManager;
	this->baseManager = baseManager;

	set<Chokepoint*> chokepoints = findInterestingChokepoints();

	assert(chokepoints.size() == 1);

	buildOrderManager->build(10, UnitTypes::Protoss_Photon_Cannon, 40);
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
}

set<Chokepoint*> DefenseManager::findInterestingChokepoints() {
  std::set<Region*> regions;  
	
  std::set<Base*> bases = baseManager->getActiveBases();

  foreach (Base* base, bases)
	  regions.insert(BWTA::getRegion(base->getBaseLocation()->getTilePosition()));

  set<Chokepoint*> chokepoints;

  foreach (Region* region, regions) {
	  foreach (Chokepoint* chokepoint, region->getChokepoints()) {
		  Region* first = chokepoint->getRegions().first;
		  Region* second = chokepoint->getRegions().second;

		  if (!regions.count(first) || !regions.count(second))
			  chokepoints.insert(chokepoint);
	}
  }

  return chokepoints;
}

std::string DefenseManager::getName() const
{
  return "Defense Manager";
}

std::string DefenseManager::getShortName() const
{
  return "Def";
}