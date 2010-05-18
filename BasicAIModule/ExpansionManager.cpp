#include "ExpansionManager.h"

ExpansionManager::ExpansionManager(Arbitrator::Arbitrator<Unit*, double>* arbitrator, BuildOrderManager* buildOrderManager, BuildManager* buildManager) {
	this->arbitrator = arbitrator;
	this->buildOrderManager = buildOrderManager;
	this->buildManager = buildManager;
}

ExpansionManager::~ExpansionManager(void) {
	delete this->arbitrator;
	delete this->buildOrderManager;
	delete this->buildManager;
}

void ExpansionManager::onOffer(set<Unit*> units) {
}

void ExpansionManager::onRevoke(Unit* unit, double bid) {
}

void ExpansionManager::update() {
}

string ExpansionManager::getName() const {
	return "Expansion Manager";
}

string ExpansionManager::getShortName() const {
	return "Expansion";
}