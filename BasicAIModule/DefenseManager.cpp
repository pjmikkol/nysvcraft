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
	bidOnMilitaryUnits();

	set<Base*> newBases = baseManager->getActiveBases();

	if (newBases != bases) {
		bases = newBases;
		interestingChokepoints = findInterestingChokepoints();
		Broodwar->printf("Found %d interesting chokepoints", interestingChokepoints.size());	
	}
	
	pair<Unit*, DefenseData> pair;

	foreach (pair, defenders)
		if (pair.second.mode == DefenseData::Idle && !interestingChokepoints.empty()) {
			pair.first->attackMove((*interestingChokepoints.begin())->getCenter());
			defenders[pair.first].mode = DefenseData::Moving;
		}
}

void DefenseManager::bidOnMilitaryUnits() {
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
	std::set<Region*> myRegions;  

	std::set<Base*> bases = baseManager->getAllBases();

	Broodwar->printf("Found %d bases", bases.size());

	foreach (Base* base, bases)
		myRegions.insert(BWTA::getRegion(base->getBaseLocation()->getTilePosition()));

	Broodwar->printf("Found %d owned regions", myRegions.size());

	set<Chokepoint*> chokepoints;

	foreach (Region* region, myRegions) {
		foreach (Chokepoint* chokepoint, region->getChokepoints()) {
			Region* first = chokepoint->getRegions().first;
			Region* second = chokepoint->getRegions().second;

			if (!myRegions.count(first) || !myRegions.count(second))
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