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
	void onRevoke(Unit* unit, double bid);

	void update();

	void onUnitShow(Unit* unit);
	void onUnitHide(Unit* unit);
	void onUnitDestroy(Unit* unit);

	string getName() const;
	string getShortName() const;
private:
	void checkBaseDefenses();
	bool isUnderAttack(Unit* base);
	void defendBase(Unit* base);
	void releaseDefenders(Unit* base);
	set<Unit*> getOurBases();
	bool hasDefenders(Unit* base);

	Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator;
	BuildOrderManager* buildOrderManager;
	BuildManager* buildManager;
	DefenseManager* defenseManager;
	map<Unit*, TroopState> attackers;
	map<Unit*, UnitGroup*> bases;
	bool builtDragoons;
};
