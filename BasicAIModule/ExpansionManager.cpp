#include "ExpansionManager.h"

using namespace BWTA;

//TODO: baseManager should not try to expand to enemy bases
//TODO: baseManager should rebuild destroyed bases
//TODO: should build assimilator to new bases
//TODO: should build defense cannons to new bases

ExpansionManager::ExpansionManager(Arbitrator::Arbitrator<Unit*, double>* arbitrator, BuildManager* buildManager,
								   BaseManager* baseManager, DefenseManager* defenseManager, WorkerManager* workerManager) {
	this->arbitrator = arbitrator;
	this->buildManager = buildManager;
	this->baseManager = baseManager;
	this->defenseManager = defenseManager;
	this->workerManager = workerManager;
	this->expansionCount = 0;
	this->lastExpanded = 0;
	this->expansionInterval = 1000;
	this->occupiedBases = set<BaseLocation*>();
	this->occupiedBases.insert(getStartLocation(Broodwar->self()));
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
	double workersPerMineral = ((double)workerManager->getWorkerCount())/mineralCount();
	Broodwar->drawTextScreen(450, 25, "workersPerMinerals: %.2f", workersPerMineral);
	int framesFromLastExpand = Broodwar->getFrameCount() - lastExpanded;
	return workersPerMineral > 1.75 && framesFromLastExpand > 2500;
}

int ExpansionManager::mineralCount() {
	int sum = 0;
	foreach (Base* base, interestingBases()) {
		sum += base->getMinerals().size();
	}
	return sum;
}

void ExpansionManager::expand() {
	BaseLocation* buildLocation = expansionLocation();
	if (!buildLocation)
		return; //no valid expansions left
	Broodwar->printf("Expand #%d", expansionCount);
	Base* expansion = baseManager->expand(buildLocation, 100);
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
	return occupiedBases.find(base) != occupiedBases.end();
}

string ExpansionManager::getName() const {
	return "Expansion Manager";
}

string ExpansionManager::getShortName() const {
	return "Expansion";
}

//TODO: should maybe keep track of nexuses instead of BaseLocations,
// since there could possibly be multiple bases close to a single BaseLocations
void ExpansionManager::onUnitShow(Unit* unit) {
	assert(unit);
	if (unit->getType() == UnitTypes::Protoss_Nexus) {
		occupiedBases.insert(baseLocation(unit));
	}
}

void ExpansionManager::onUnitDestroy(Unit* unit) {
	assert(unit);
	if (unit->getType() == UnitTypes::Protoss_Nexus) {
		occupiedBases.erase(baseLocation(unit));
	}
}

BaseLocation* ExpansionManager::baseLocation(Unit* unit) {
	Region* reg = BWTA::getRegion(unit->getTilePosition());
	set<BaseLocation*> baseLocations = reg->getBaseLocations();
	return *(baseLocations.begin());
}

//bases that are either active or being constructed
set<Base*> ExpansionManager::interestingBases() {
	set<Base*> bases = set<Base*>();
	foreach (Base* base, this->baseManager->getAllBases()) {
		if (base->isActive() || base->isBeingConstructed())
			bases.insert(base);
	}
	return bases;
}