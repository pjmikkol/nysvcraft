#include "ArmyManager.h"

using namespace BWAPI;
using namespace std;

ArmyManager::ArmyManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator, BuildOrderManager* buildOrderManager) {
	this->arbitrator = arbitrator;
	this->buildOrderManager = buildOrderManager;

	buildOrderManager->build(20, UnitTypes::Protoss_Zealot, 80);
}

ArmyManager::~ArmyManager()
{
	delete this->buildOrderManager;
	delete this->arbitrator;
}

void ArmyManager::onOffer(set<Unit*> units) {
	foreach (Unit* unit, units) {
		if (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Dragoon) {
			this->arbitrator->accept(this, unit);		
			Broodwar->printf("Attacking with new %s unit.", unit->getType().getName());
			unit->attackMove(Position(70 * TILE_SIZE, 8 * TILE_SIZE));
		}
	}
}

void ArmyManager::onRevoke(Unit* unit, double bid) {
}

void ArmyManager::update() {
}

void ArmyManager::onUnitDestroy(Unit* unit) {
}

void ArmyManager::onUnitShow(Unit* unit) {
}

void ArmyManager::onUnitHide(Unit* unit) {
}

string ArmyManager::getName() const {
	return "Army Manager";
}

string ArmyManager::getShortName() const {
	return "Army";
}