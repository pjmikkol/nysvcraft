#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <Arbitrator.h>
#include <WorkerManager.h>
#include <SupplyManager.h>
#include <BuildManager.h>
#include <BuildOrderManager.h>
#include <TechManager.h>
#include <UpgradeManager.h>
#include <BaseManager.h>
#include <ScoutManager.h>
#include <DefenseManager.h>
#include <InformationManager.h>
#include <UnitGroupManager.h>
#include <EnhancedUI.h>
#include <windows.h>
#include <boost/foreach.hpp>

static bool analyzed;

class BasicAIModule : public BWAPI::AIModule
{
public:
	virtual void onStart();
	virtual void onFrame();
	virtual void onUnitShow(BWAPI::Unit* unit);
	virtual void onUnitHide(BWAPI::Unit* unit);
	virtual void onUnitMorph(BWAPI::Unit* unit);
	virtual void onUnitRenegade(BWAPI::Unit* unit);
	virtual void onUnitDestroy(BWAPI::Unit* unit);
	virtual bool onSendText(std::string text);
	void showStats(); //not part of BWAPI::AIModule
	void showPlayers();
	void showForces();
	std::map<BWAPI::Unit*,BWAPI::UnitType> buildings;
	Arbitrator::Arbitrator<BWAPI::Unit*,double> arbitrator;
	WorkerManager* workerManager;
	SupplyManager* supplyManager;
	BuildManager* buildManager;
	TechManager* techManager;
	UpgradeManager* upgradeManager;
	BaseManager* baseManager;
	ScoutManager* scoutManager;
	BuildOrderManager* buildOrderManager;
	DefenseManager* defenseManager;
	InformationManager* informationManager;
	UnitGroupManager* unitGroupManager;
	EnhancedUI* enhancedUI;
	bool showManagerAssignments;
};