#include "ExpansionManager.h"

ExpansionManager::ExpansionManager(Arbitrator::Arbitrator<Unit*, double>* arbitrator, BuildManager* buildManager, BaseManager* baseManager) {
	this->arbitrator = arbitrator;
	this->buildManager = buildManager;
	this->baseManager = baseManager;
	this->expansionCount = 0;
	this->lastExpanded = 0;
	this->expansionInterval = 1000;
}

ExpansionManager::~ExpansionManager(void) {
}

void ExpansionManager::onOffer(set<Unit*> units) {
}

void ExpansionManager::onRevoke(Unit* unit, double bid) {
}

void ExpansionManager::update() {
	int timeFromLastExpansion = Broodwar->getFrameCount() - lastExpanded;
	if (timeFromLastExpansion > expansionInterval &&
		buildManager->getCompletedCount(UnitTypes::Protoss_Pylon) >= 2*(expansionCount + 1)) {
		Broodwar->printf("Expand #%d", expansionCount);
		baseManager->expand();
		expansionCount++;
		lastExpanded = Broodwar->getFrameCount();
		expansionInterval /= 2;
	}
}

string ExpansionManager::getName() const {
	return "Expansion Manager";
}

string ExpansionManager::getShortName() const {
	return "Expansion";
}