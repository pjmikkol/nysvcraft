#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
#include <algorithm>
#include "BuildOrderManager.h"
#include "BaseManager.h"
#include "BaseManager/Base.h"
#include "UnitGroupManager.h"
#include "UnitGroup.h"
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

class DefenseManager : Arbitrator::Controller<BWAPI::Unit*,double>
{
public:
	class DefenseData
	{
	public:
		enum DefenseMode
		{
			Idle,
			Moving
		};
		DefenseData(){ mode = Idle; }
		DefenseMode mode;
	};
	DefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator, BuildOrderManager* buildOrderManager, BaseManager* baseManager);
	virtual void onOffer(std::set<BWAPI::Unit*> units);
	virtual void onRevoke(BWAPI::Unit* unit, double bid);
	void onUnitDestroy(BWAPI::Unit* unit);
	void onRemoveUnit(BWAPI::Unit* unit);
	void onExpand(Base* newBase);
	void onUnitShow(BWAPI::Unit* unit);
	virtual void update();
	virtual std::string getName() const;
	virtual std::string getShortName() const;	

private:
	void bidOnMilitaryUnits();
	void checkInterestingChokepoints();
	void giveDefenseOrders();
	std::set<BWAPI::Unit*> getIdleDefenders();
	void releaseDefenseGroupAt(BWTA::Chokepoint* chokepoint);
	void updateExploredRegions();
	void addInterestingChokepoint(BWTA::Chokepoint* chokepoint);
	void removeInterestingChokepoint(BWTA::Chokepoint* chokepoint);

	bool isBaseRegion(BWTA::Region* region);
	bool isUnexplored(BWTA::Region* region);
	bool isExplored(BWTA::Region* region);

	bool calledInitialOnExpand;

	BuildOrderManager* buildOrderManager;
	BaseManager* baseManager;
	Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
	std::map<BWAPI::Unit*,DefenseData> defenders;
	std::set<BWTA::Chokepoint*> interestingChokepoints;
	std::map<BWTA::Chokepoint*, UnitGroup*> defenseGroups;
	std::set<BWTA::Region*> exploredRegions;
};