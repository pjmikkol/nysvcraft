#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BaseManager.h>
#include <BuildManager.h>
#include <UnitGroup.h>
#include <boost/foreach.hpp>
#include <DefenseManager.h>
#define foreach BOOST_FOREACH

using namespace BWAPI;
using namespace std;

class ArmyManager :
	public Arbitrator::Controller<BWAPI::Unit*, double>
{
public:
	ArmyManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator, BuildOrderManager* buildOrderManager, BuildManager* buildManager, DefenseManager* defenseManager);
	~ArmyManager();

	enum TroopState { IdleTroop, AttackingTroop };

	void onOffer(set<Unit*> units);

	Position getRushTarget();

	void onRevoke(Unit* unit, double bid);

	void update();

	void onUnitShow(Unit* unit);
	void onUnitHide(Unit* unit);
	void onUnitDestroy(Unit* unit);

	string getName() const;
	string getShortName() const;

	void rush();

private:
	void checkBaseDefenses();
	bool isUnderAttack(Unit* base);
	void defendBase(Unit* base);
	void releaseDefenders(Unit* base);
	set<Unit*> getOurBases();
	void releaseAttackers(Unit* base);
	void attack(Unit* target);

	Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator;
	BuildOrderManager* buildOrderManager;
	BuildManager* buildManager;
	DefenseManager* defenseManager;
	bool builtDragoons;

	bool isRush;

	set<Unit*> recalled;
	set<Unit*> recalledAttackers;
	set<Unit*> defenders;
	set<Unit*> attackers;
	set<Unit*> bases;
	set<Unit*> attackBases;
};
