#include "ArmyManager.h"
#include <algorithm>

using namespace BWAPI;
using namespace std;
using namespace BWTA;

ArmyManager::ArmyManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator, BuildOrderManager* buildOrderManager, BuildManager* buildManager, DefenseManager* defenseManager) {
	this->arbitrator = arbitrator;
	this->buildOrderManager = buildOrderManager;
	this->buildManager = buildManager;
	this->defenseManager = defenseManager;

	builtDragoons = false;

	buildOrderManager->build(999999, UnitTypes::Protoss_Zealot, 70);
	buildOrderManager->build(2, UnitTypes::Protoss_Gateway, 70);
}

ArmyManager::~ArmyManager()
{
}

void ArmyManager::onOffer(set<Unit*> units) {
	foreach (Unit* unit, units) {
		pair<Unit*, UnitGroup*> pair;

		foreach (pair, bases)
			if (pair.second->count(unit)) {
				arbitrator->accept(this, unit, 100);		
				unit->attackMove(pair.first->getPosition());
			}

		//arbitrator->decline(this, unit);
	}
}

void ArmyManager::onRevoke(Unit* unit, double bid) {
	attackers.erase(unit);
	
	pair<Unit*, UnitGroup*> pair;

	foreach (pair, bases)
		pair.second->erase(unit);
}

void ArmyManager::update() {
	set<Unit*> units = BWAPI::Broodwar->self()->getUnits();
	
	int completedZealots = buildManager->getCompletedCount(UnitTypes::Protoss_Zealot);

	if (!builtDragoons && completedZealots == 6) {
		buildOrderManager->build(999999, UnitTypes::Protoss_Dragoon, 70);
		builtDragoons = true;
	}
	
	pair<Unit*, UnitGroup*> pair;

	foreach (pair, bases)
		foreach (Unit* unit, *pair.second)
			arbitrator->setBid(this, unit, 100);

	foreach (Unit* unit, units) 
		if (unit->isCompleted() && unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Dragoon)
			arbitrator->setBid(this, unit, 10);

	checkBaseDefenses();
}

void ArmyManager::checkBaseDefenses() {
	foreach (Unit* base, getOurBases()) {
		if (isUnderAttack(base) && !hasDefenders(base)) 
			defendBase(base);
		else if (!isUnderAttack(base))
			releaseDefenders(base);
	}
}

bool ArmyManager::isUnderAttack(Unit* base) {
	foreach (Unit* enemy, Broodwar->enemy()->getUnits()) {
		if (enemy->getTarget() == base || enemy->getOrderTarget() == base || base->getDistance(enemy) < 600) {
			return true;
		}
	}

	return false;
}

bool ArmyManager::hasDefenders(Unit* base) {
	return bases.count(base) && bases[base]->size() > 4;
}

bool sortByUnitCount(UnitGroup* a, UnitGroup* b) {
	return a->size() < b->size();
}

void ArmyManager::onUnitDestroy(Unit* unit) {
	onRevoke(unit, 9999);
}

void ArmyManager::defendBase(Unit* base) {
	Broodwar->printf("Defend base at %d, %d", base->getTilePosition().x(), base->getTilePosition().y());

	set<UnitGroup*> defGroups = defenseManager->getDefenseGroups();

	UnitGroup* defenders = new UnitGroup();	

	foreach (UnitGroup* group, defGroups)
		foreach (Unit* unit, *group) {
			arbitrator->setBid(this, unit, 100);
			defenders->insert(unit);
		}

	Broodwar->printf("Recall %d defenders", defenders->size());

	bases.insert(make_pair(base, defenders));
}

void ArmyManager::releaseDefenders(Unit* base) {
	if (bases.count(base)) {
		UnitGroup* group = bases[base];
		
		foreach (Unit* unit, *bases[base])
			onRevoke(unit, 9999);

		bases.erase(base);		

		delete group;
	}
}

set<Unit*> ArmyManager::getOurBases() {
	set<Unit*> ourBases;

	foreach (Unit* unit, Broodwar->self()->getUnits())
		if (unit->getType().isBuilding())
			ourBases.insert(unit);

	return ourBases;
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