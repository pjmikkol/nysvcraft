#include "ArmyManager.h"
#include <algorithm>
#include "Helpers.h"
#include <ctime>

using namespace BWAPI;
using namespace std;
using namespace BWTA;

ArmyManager::ArmyManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator, BuildOrderManager* buildOrderManager, BuildManager* buildManager, DefenseManager* defenseManager) {
	this->arbitrator = arbitrator;
	this->buildOrderManager = buildOrderManager;
	this->buildManager = buildManager;
	this->defenseManager = defenseManager;

	builtDragoons = false;
	isRush = false;

	buildOrderManager->build(999999, UnitTypes::Protoss_Zealot, 70);
	buildOrderManager->build(2, UnitTypes::Protoss_Gateway, 70);
}

ArmyManager::~ArmyManager()
{
}

void ArmyManager::rush() {
	Broodwar->printf("Kekekekekeke");
	isRush = true;
	rushStartedOn = time(NULL);
}

void ArmyManager::onOffer(set<Unit*> units) {	
	foreach (Unit* unit, units) {
		if (isRush) {
			unit->attackMove(getRushTarget());
			continue;
		} 
		if (recalledAttackers.count(unit)) {
			arbitrator->accept(this, unit, 200);
			if (!attackBases.empty())
				unit->attackMove((*attackBases.begin())->getPosition());
			recalledAttackers.erase(unit);
			attackers.insert(unit);
		} else if (recalled.count(unit)) {
			arbitrator->accept(this, unit, 100);		
			if (!bases.empty())
				unit->attackMove((*bases.begin())->getPosition());			
			recalled.erase(unit);
			defenders.insert(unit);
		} 
	}
}

Position ArmyManager::getRushTarget() {
	if (Broodwar->self()->getStartLocation().y() == 8)
		return Position(63*TILE_SIZE, 117*TILE_SIZE);
	else
		return Position(70*TILE_SIZE, 8*TILE_SIZE);
}

void ArmyManager::onRevoke(Unit* unit, double bid) {
	recalled.erase(unit);
	defenders.erase(unit);		
	recalledAttackers.erase(unit);
	attackers.erase(unit);
}

void ArmyManager::update() {
	set<Unit*> units = BWAPI::Broodwar->self()->getUnits();

	if (isRush && time(NULL) >= rushStartedOn + 1.5*60)
		isRush = false;
	
	int completedZealots = buildManager->getCompletedCount(UnitTypes::Protoss_Zealot);

	if (!builtDragoons && completedZealots == 6) {
		buildOrderManager->build(999999, UnitTypes::Protoss_Dragoon, 70);
		builtDragoons = true;
	}

	foreach (Unit* unit, defenders)
		arbitrator->setBid(this, unit, 100);

	foreach (Unit* unit, attackers) {
		arbitrator->setBid(this, unit, 200);

		if (unit->isIdle())
			unit->attackMove((*attackBases.begin())->getPosition());
	}

	foreach (Unit* unit, units) 
		if (unit->isCompleted() && unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Dragoon)			
			if (!(attackers.count(unit) || recalledAttackers.count(unit) || recalled.count(unit) || defenders.count(unit))) {
				if (isRush && !(unit->getOrderTarget() || unit->getTarget()))
					unit->attackMove(getRushTarget());
				else
					if (!attackBases.empty()) {
						attack(*attackBases.begin());
					} else 
						arbitrator->setBid(this, unit, 10);
			}

	checkBaseDefenses();
}


void ArmyManager::checkBaseDefenses() {
	if (isRush) return;

	foreach (Unit* base, getOurBases()) {
		if (isUnderAttack(base)) 
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

bool sortByUnitCount(UnitGroup* a, UnitGroup* b) {
	return a->size() < b->size();
}

void ArmyManager::onUnitDestroy(Unit* unit) {
	if (attackBases.count(unit))
		releaseAttackers(unit);
	
	onRevoke(unit, 9999);
}

void ArmyManager::defendBase(Unit* base) {
	set<UnitGroup*> defGroups = defenseManager->getDefenseGroups();

	bases.insert(base);

	foreach (UnitGroup* group, defGroups)
		foreach (Unit* unit, *group) {
			if (attackers.count(unit) || recalledAttackers.count(unit))
				continue;
			arbitrator->setBid(this, unit, 100);	
			recalled.insert(unit);
		}
}

void ArmyManager::releaseDefenders(Unit* base) {
	foreach (Unit* unit, defenders)
		onRevoke(unit, 9999);
	foreach (Unit* unit, recalled)
		onRevoke(unit, 9999);
	bases.erase(base);
}

void ArmyManager::releaseAttackers(Unit* base) {
	foreach (Unit* unit, attackers)
		onRevoke(unit, 9999);
	foreach (Unit* unit, recalledAttackers)
		onRevoke(unit, 9999);
	attackBases.erase(base);
}

set<Unit*> ArmyManager::getOurBases() {
	set<Unit*> ourBases;

	foreach (Unit* unit, Broodwar->self()->getUnits())
		if (unit->getType().isBuilding())
			ourBases.insert(unit);

	return ourBases;
}

void ArmyManager::onUnitShow(Unit* unit) {
	if (unit->getType().isBuilding() && unit->getPlayer() == Broodwar->enemy()) {
		Broodwar->printf("Saw enemy building!");

		Unit* closestUnit = helpers::getClosestUnitFrom(unit->getPosition(), Broodwar->self()->getUnits());

		if (closestUnit->getType() == UnitTypes::Protoss_Dragoon || closestUnit->getType() == UnitTypes::Protoss_Zealot) {
			Broodwar->printf("THIS IS SPARTAAAAAAAAA!!!!!!!!!");
			attack(unit);
		}
	}
}

void ArmyManager::attack(Unit* target) {
	set<UnitGroup*> defGroups = defenseManager->getDefenseGroups();

	foreach (UnitGroup* group, defGroups)
		foreach (Unit* unit, *group) {
			arbitrator->setBid(this, unit, 200);
			if (!attackers.count(unit)) {
				recalledAttackers.insert(unit);
				defenders.erase(unit);
				recalled.erase(unit);
			}
			attackBases.insert(target);
		}
}

void ArmyManager::onUnitHide(Unit* unit) {
}

string ArmyManager::getName() const {
	return "Army Manager";
}

string ArmyManager::getShortName() const {
	return "Army";
}