#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BaseManager.h>
#include <BuildManager.h>
#include "Helpers.h"
#include <cassert>
#include <algorithm>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace BWAPI;
using namespace std;
using namespace helpers;

class BattleManager :
	Arbitrator::Controller< BWAPI::Unit*, double>
{
public:
	/* Would maybe be wise to give references to army- & defense-managers */
	BattleManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator);
	~BattleManager(void);

	void onOffer(set<Unit*> units);
	void onRevoke(Unit* unit, double bid);

	void update();

	void onUnitShow(Unit* unit);
	void onUnitHide(Unit* unit);
	void onUnitDestroy(Unit* unit);
	void onRemoveUnit(Unit* unit);

	string getName() const;
	string getShortName() const;

	void printAttackerInfo(map<Unit*, set<Unit*> >* attackedBy);
	void decideActions(map<Unit*, set<Unit*> >* attackedBy);
	void handleFlee(Unit* unit, set<Unit*> attackedBy);
	Position fleeTo(Unit* unit, const set<Unit*>* attackers);
	set<double>* calculateAngles(Unit* unit, const set<Unit*>* attackers);
	double calculateAngle(Unit* unit, Unit* enemy);
	double midAngle(set<double>* angles);
	double reverseAngle(double angle);
	void handleAttack(Unit* unit);
	void calculateTarget(Unit* unit, set<Unit*> enemies);
	Unit* weakestEnemyInRange(Unit* unit, set<Unit*> enemies);
	map< Unit*, set<Unit*> > * getAttackers();
	bool isInAttackRange(Unit* attacker, Unit* target);
	void drawUnitInfo();
	void BidUnits();
	bool doWeWantUnit(Unit* unit);
	void setUnitsFree();
	bool canWeReleaseUnit(Unit* u);

private:
	Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator;
	map<Unit*, UnitData>* fighters;
};


