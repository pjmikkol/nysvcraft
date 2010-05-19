#include "ExpansionManager.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using namespace BWTA;

//TODO: baseManager should not try to expand to enemy bases
//TODO: baseManager should rebuild destroyed bases
//TODO: should build assimilator to new bases
//TODO: should build defense cannons to new bases

ExpansionManager::ExpansionManager(Arbitrator::Arbitrator<Unit*, double>* arbitrator, BuildManager* buildManager,
								   BaseManager* baseManager, DefenseManager* defenseManager) {
	this->arbitrator = arbitrator;
	this->buildManager = buildManager;
	this->baseManager = baseManager;
	this->expansionCount = 0;
	this->lastExpanded = 0;
	this->expansionInterval = 1000;
	this->defenseManager = defenseManager;
}

ExpansionManager::~ExpansionManager(void) {
}

void ExpansionManager::onOffer(set<Unit*> units) {
}

void ExpansionManager::onRevoke(Unit* unit, double bid) {
}

void ExpansionManager::update() {
	if (shouldExpand()) {
		expand();
	}
}

bool ExpansionManager::shouldExpand() {
	int timeFromLastExpansion = Broodwar->getFrameCount() - lastExpanded;
	return timeFromLastExpansion > expansionInterval &&
	       buildManager->getCompletedCount(UnitTypes::Protoss_Pylon) >= 2*(expansionCount + 1);
}
void ExpansionManager::expand() {
	BaseLocation* expansionLocationPenis = expansionLocation();
	if (!expansionLocationPenis)
		return; //no valid expansions left
	Broodwar->printf("Expand #%d", expansionCount);
	Base* expansion = baseManager->expand();
	expansionCount++;
	lastExpanded = Broodwar->getFrameCount();
	expansionInterval /= 2;
	defenseManager->onExpand(expansion);
}

/*
 * finds nearest free BaseLocation, return NULL if none available
 */
BaseLocation* ExpansionManager::expansionLocation() {
	BWTA::BaseLocation* location = NULL;
	double minDist = -1;
	BWTA::BaseLocation* home = BWTA::getStartLocation(BWAPI::Broodwar->self());
	foreach (BWTA::BaseLocation* base, BWTA::getBaseLocations()) {
		if (occupied(base))
			continue;
		double distance = home->getGroundDistance(base);
		if (minDist == -1 || distance < minDist) {
			minDist = distance;
			location = base;
		}
	}
	return location;
}

bool ExpansionManager::occupied(BaseLocation* base) {
	return false; /*
	set<BaseLocation*> enemyBases = getEnemyBases();
	set<BaseLocation*> myBases    = getMyBases();
	set<BaseLocation*> occupiedBases = getOccupiedBases();
	return !enemyBases->find(base) && !myBases->find(base); */
}

set<BaseLocation*>* getEnemyBases() {
	return NULL; //FIXME

	/*
	set<BaseLocation*> freeBases = BWTA::getBaseLocations();
	set<Unit*> enemyUnits = Broodwar->enemy()->getUnits();
	set<Unit*> allUnits;
	foreach	(Unit* unit, enemyUnits) {
		if (unit->getType() == UnitTypes::Protoss_Nexus) {
			BWTA::BaseLocation* base = getNearestBaseLocation(unit->getTilePosition());
			if (freeBases.find(base)) {
				freeBases.erase(base);
			}
		}
	}

	*/
}

string ExpansionManager::getName() const {
	return "Expansion Manager";
}

string ExpansionManager::getShortName() const {
	return "Expansion";
}