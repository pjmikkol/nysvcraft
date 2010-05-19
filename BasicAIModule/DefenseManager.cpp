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

	updateExploredRegions();

	checkInterestingChokepoints();
	
	giveDefenseOrders();
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

void DefenseManager::updateExploredRegions() {
	foreach (Unit* unit, Broodwar->self()->getUnits()) {
		Region* region = BWTA::getRegion(unit->getTilePosition());
		if (!isBaseRegion(region))
			exploredRegions.insert(region);
	}
}

bool DefenseManager::isBaseRegion(Region* region) {
	foreach (Base* base, baseManager->getActiveBases())
		if (BWTA::getRegion(base->getBaseLocation()->getTilePosition()) == region)
			return true;

	return false;
}

void DefenseManager::checkInterestingChokepoints() {
	set<Base*> newBases = baseManager->getActiveBases();

	if (newBases != bases) {
		Base* newBase;

		foreach (Base* base, newBases)
			if (!bases.count(base)) {
				newBase = base;
				break;
			}

		assert(newBase != NULL);

		Region* region = getRegion(newBase->getBaseLocation()->getTilePosition());

		foreach (Chokepoint* chokepoint, region->getChokepoints()) {
			Region* other;

			if (chokepoint->getRegions().first == region)
				other = chokepoint->getRegions().second;
			else
				other = chokepoint->getRegions().first;

			if (isUnexplored(other))
				addInterestingChokepoint(chokepoint);
			else if (isBaseRegion(other))
				removeInterestingChokepoint(chokepoint);
			else if (isExplored(other)) {
				foreach (Chokepoint* otherChokepoint, other->getChokepoints()) {
					removeInterestingChokepoint(otherChokepoint);			
					
					Region* neighbour = otherChokepoint->getRegions().first == other ? otherChokepoint->getRegions().second : otherChokepoint->getRegions().first;

					if (isUnexplored(neighbour))
						addInterestingChokepoint(otherChokepoint);
				}
			}
		}
				
		bases = newBases;		
	
		Broodwar->printf("Found %d interesting chokepoints", interestingChokepoints.size());	
	}
}

bool DefenseManager::isUnexplored(Region* region) {
	return !isExplored(region);
}

bool DefenseManager::isExplored(Region* region) {
	return exploredRegions.count(region) == 1;
}

void DefenseManager::addInterestingChokepoint(Chokepoint* chokepoint) {
	interestingChokepoints.insert(chokepoint);
	defenseGroups.insert(make_pair(chokepoint, new UnitGroup()));
}

void DefenseManager::removeInterestingChokepoint(Chokepoint* chokepoint) {
	interestingChokepoints.erase(chokepoint);
	releaseDefenseGroupAt(chokepoint);
}

void DefenseManager::releaseDefenseGroupAt(Chokepoint* chokepoint) {
	Broodwar->printf("Release defenders at %d, %d", chokepoint->getCenter().x(), chokepoint->getCenter().y());

	UnitGroup* group = defenseGroups[chokepoint];

	foreach (Unit* unit, *group)
		defenders[unit].mode = DefenseData::Idle;

	defenseGroups.erase(chokepoint);
}

bool sortByUnitCount(pair<Chokepoint*, UnitGroup*> a, pair<Chokepoint*, UnitGroup*> b) {
	return a.second->size() < b.second->size();
}

void DefenseManager::giveDefenseOrders() {
	set<Unit*> idleUnits = getIdleDefenders();

	foreach (Unit* defender, idleUnits) {
		vector< pair<Chokepoint*, UnitGroup*> > defGroups = 
			vector< pair<Chokepoint*, UnitGroup*> >(defenseGroups.begin(), defenseGroups.end());

		sort(defGroups.begin(), defGroups.end(), sortByUnitCount);

		Chokepoint* target = (*defGroups.begin()).first;
		UnitGroup* group   = (*defGroups.begin()).second;

		group->insert(defender);
		defender->attackMove(target->getCenter());
		defenders[defender].mode = DefenseData::Moving;
	}
}

set<Unit*> DefenseManager::getIdleDefenders() {
	set<Unit*> idleDefenders;

	pair<Unit*, DefenseData> pair;

	foreach (pair, defenders)
		if (pair.second.mode == DefenseData::Idle)
			idleDefenders.insert(pair.first);

	return idleDefenders;
}

std::string DefenseManager::getName() const
{
	return "Defense Manager";
}

std::string DefenseManager::getShortName() const
{
	return "Def";
}