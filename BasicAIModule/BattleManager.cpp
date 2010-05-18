#include "BattleManager.h"

using namespace std;

BattleManager::BattleManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator)
{
	this->arbitrator = arbitrator;
	this->fighters = new map<Unit*, UnitData>();
}

BattleManager::~BattleManager(void)
{
	delete this->fighters;
}

void BattleManager::onOffer(set<Unit*> units)
{
	foreach (Unit* unit, units) {
		if (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Dragoon) {
			this->arbitrator->accept(this, unit);
			UnitData ud = { fight, 0, 0 };
			this->fighters->insert(make_pair(unit, ud));
		}
	}
}

void BattleManager::onRevoke(Unit* unit, double bid)
{

}

void BattleManager::update()
{
	/*Micro-level attack code goes here*/
}

void BattleManager::onUnitShow(Unit* unit)
{

}

void BattleManager::onUnitHide(Unit* unit)
{

}

void BattleManager::onUnitDestroy(Unit* unit)
{

}

string BattleManager::getName() const
{
	return "Battle Manager";
}

string BattleManager::getShortName() const
{
	return "Battle";
}

