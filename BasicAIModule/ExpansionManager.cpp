#include "ExpansionManager.h"

ExpansionManager::ExpansionManager(Arbitrator::Arbitrator<Unit*, double>* arbitrator, BuildManager* buildManager, BaseManager* baseManager) {
	this->arbitrator = arbitrator;
	this->buildManager = buildManager;
	this->baseManager = baseManager;
	this->expansionCount = 0;
}

ExpansionManager::~ExpansionManager(void) {
}

void ExpansionManager::onOffer(set<Unit*> units) {
}

void ExpansionManager::onRevoke(Unit* unit, double bid) {
}

void ExpansionManager::update() {
	if (expansionCount == 0 && buildManager->getCompletedCount(UnitTypes::Protoss_Pylon) == 3) {
		Broodwar->printf("Expand #%d", expansionCount);
		baseManager->expand();
		expansionCount++;
	}
}

string ExpansionManager::getName() const {
	return "Expansion Manager";
}

string ExpansionManager::getShortName() const {
	return "Expansion";
}