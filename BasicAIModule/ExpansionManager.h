#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BaseManager.h>
#include <BuildManager.h>
#include "DefenseManager.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace BWAPI;
using namespace std;

class ExpansionManager :
	public Arbitrator::Controller<BWAPI::Unit*, double>
{
public:
	ExpansionManager(Arbitrator::Arbitrator<Unit*, double>* arbitrator, BuildManager* buildManager, BaseManager* baseManager, DefenseManager* defenseManager);
	~ExpansionManager();

	void onOffer(set<Unit*> units);
	void onRevoke(Unit* unit, double bid);

	void update();

	void onUnitShow(Unit* unit);
	void onUnitHide(Unit* unit);
	void onUnitDestroy(Unit* unit);

	string getName() const;
	string getShortName() const;
private:
	Arbitrator::Arbitrator<Unit*, double>* arbitrator;
	BuildManager* buildManager;
	BaseManager* baseManager;
	DefenseManager* defenseManager;
	int expansionCount;
};
