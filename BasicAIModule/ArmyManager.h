#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BaseManager.h>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace BWAPI;
using namespace std;

class ArmyManager :
	public Arbitrator::Controller<BWAPI::Unit*, double>
{
public:
	ArmyManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator, BuildOrderManager* buildOrderManager);
	~ArmyManager();

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
	BuildOrderManager* buildOrderManager;
};
