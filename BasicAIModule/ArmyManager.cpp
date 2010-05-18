#include "ArmyManager.h"

using namespace BWAPI;
using namespace std;

ArmyManager::ArmyManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator, BuildOrderManager* buildOrderManager) {
	this->arbitrator = arbitrator;
	this->buildOrderManager = buildOrderManager;

	buildOrderManager->build(20, UnitTypes::Protoss_Zealot, 70);
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
			attackers.insert(make_pair(unit, IdleTroop));
		}
	}
}

void ArmyManager::onRevoke(Unit* unit, double bid) {
}

void ArmyManager::update() {
	set<Unit*> units = BWAPI::Broodwar->self()->getUnits();
	
	foreach (Unit* unit, units) 
		if (unit->getType() == UnitTypes::Protoss_Zealot)
			arbitrator->setBid(this, unit, 20);    

	pair<Unit*, TroopState> pair;

	foreach (pair, attackers) {
		Unit* attacker = pair.first;
		TroopState state = pair.second;

		if (state == IdleTroop) {
			Broodwar->printf("Attacking with new %s unit.", attacker->getType().getName().c_str());
			attacker->attackMove(Position(70 * TILE_SIZE, 8 * TILE_SIZE));
			attackers[attacker] = AttackingTroop;
		}
	}
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