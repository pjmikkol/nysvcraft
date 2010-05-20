#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BaseManager.h>
#include <BuildManager.h>
#include <DefenseManager.h>
#include <WorkerManager.h>
#include <boost/foreach.hpp>
#include <cassert>
#include <fstream>
#define foreach BOOST_FOREACH

using namespace BWAPI;
using namespace BWTA;
using namespace std;

class ExpansionManager :
	public Arbitrator::Controller<BWAPI::Unit*, double>
{
public:
	ExpansionManager(Arbitrator::Arbitrator<Unit*, double>* arbitrator, BuildManager* buildManager,
		BaseManager* baseManager, DefenseManager* defenseManager, WorkerManager* workerManager);
	~ExpansionManager();

	void onOffer(set<Unit*> units);
	void onRevoke(Unit* unit, double bid);

	void update();

	void onUnitShow(Unit* unit);
	void onUnitHide(Unit* unit);
	void onUnitDestroy(Unit* unit);

	string getName() const;
	string getShortName() const;

	void expand();
private:
	bool shouldExpand();
	BaseLocation* expansionLocation();
	bool occupied(BaseLocation*);
	int mineralCount();

	BaseLocation* baseLocation(Unit* unit);

	Arbitrator::Arbitrator<Unit*, double>* arbitrator;
	BuildManager* buildManager;
	BaseManager* baseManager;
	DefenseManager* defenseManager;
	WorkerManager* workerManager;
	int expansionCount;
	int lastExpanded;
	int expansionInterval;

	set<BaseLocation*> occupiedBases;
};
