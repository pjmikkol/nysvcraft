#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BaseManager.h>
#include <BuildManager.h>
#include "Helpers.h"
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

	string getName() const;
	string getShortName() const;

private:
	Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator;
	map<Unit*, UnitData>* fighters;
};


