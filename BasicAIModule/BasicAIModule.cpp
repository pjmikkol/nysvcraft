#include "BasicAIModule.h"
using namespace BWAPI;

DWORD WINAPI AnalyzeThread(void* obj) {
	BWTA::readMap();
	BWTA::analyze();

	BasicAIModule* ai = (BasicAIModule*) obj;

	ai->buildManager       = new BuildManager(&ai->arbitrator);
	ai->techManager        = new TechManager(&ai->arbitrator);
	ai->upgradeManager     = new UpgradeManager(&ai->arbitrator);
	ai->scoutManager       = new ScoutManager(&ai->arbitrator);
	ai->workerManager      = new WorkerManager(&ai->arbitrator);
	ai->buildOrderManager  = new BuildOrderManager(ai->buildManager,ai->techManager,ai->upgradeManager,ai->workerManager);
	ai->baseManager        = new BaseManager();
	ai->supplyManager      = new SupplyManager();
	ai->defenseManager     = new DefenseManager(&ai->arbitrator, ai->buildOrderManager, ai->baseManager);
	ai->informationManager = new InformationManager();
	ai->unitGroupManager   = new UnitGroupManager();
	ai->enhancedUI         = new EnhancedUI();
	ai->armyManager		   = new ArmyManager(&ai->arbitrator, ai->buildOrderManager, ai->buildManager);
	ai->battleManager      = new BattleManager(&ai->arbitrator);
	ai->expansionManager   = new ExpansionManager(&ai->arbitrator, ai->buildManager, ai->baseManager, ai->defenseManager, ai->workerManager);

	ai->supplyManager->setBuildManager(ai->buildManager);
	ai->supplyManager->setBuildOrderManager(ai->buildOrderManager);
	ai->techManager->setBuildingPlacer(ai->buildManager->getBuildingPlacer());
	ai->upgradeManager->setBuildingPlacer(ai->buildManager->getBuildingPlacer());
	ai->workerManager->setBaseManager(ai->baseManager);
	ai->workerManager->setBuildOrderManager(ai->buildOrderManager);
	ai->baseManager->setBuildOrderManager(ai->buildOrderManager);

	ai->buildOrderManager->enableDependencyResolver();
	ai->workerManager->enableAutoBuild();
	ai->workerManager->setAutoBuildPriority(80); 

	ai->scoutManager->setScoutCount(1);
	
	analyzed = true;

	return 0;
}

void BasicAIModule::onStart()
{
	Broodwar->setLocalSpeed(50);

	analyzed = false;

	this->showManagerAssignments = false;

	if (Broodwar->isReplay()) return;

	Broodwar->enableFlag(Flag::UserInput);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) AnalyzeThread, (void*) this, 0, NULL);
}

void BasicAIModule::onFrame()
{
	if (Broodwar->isReplay()) return;
	if (!analyzed) return;

	this->buildManager->update();
	this->buildOrderManager->update();
	this->baseManager->update();
	this->workerManager->update();
	this->techManager->update();
	this->upgradeManager->update();
	this->supplyManager->update();
	this->scoutManager->update();
	this->defenseManager->update();
	this->armyManager->update();
	this->expansionManager->update();
	this->battleManager->update();
	this->arbitrator.update();

	this->enhancedUI->update();

	std::set<Unit*> units=Broodwar->self()->getUnits();
	if (this->showManagerAssignments)
	{
		for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++)
		{
			if (this->arbitrator.hasBid(*i))
			{
				int x=(*i)->getPosition().x();
				int y=(*i)->getPosition().y();
				std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> > bids=this->arbitrator.getAllBidders(*i);
				int y_off=0;
				bool first = false;
				const char activeColor = '\x07', inactiveColor = '\x16';
				char color = activeColor;
				for(std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> >::iterator j=bids.begin();j!=bids.end();j++)
				{
					Broodwar->drawText(CoordinateType::Map,x,y+y_off,"%c%s: %d",color,j->first->getShortName().c_str(),(int)j->second);
					y_off+=15;
					color = inactiveColor;
				}
			}
		}
	}

	UnitGroup myPylonsAndGateways = SelectAll()(Pylon,Gateway)(HitPoints,"<=",200);
	for each(Unit* u in myPylonsAndGateways)
	{
		Broodwar->drawCircleMap(u->getPosition().x(),u->getPosition().y(),20,Colors::Red);
	}
}

void BasicAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	this->arbitrator.onRemoveObject(unit);
	this->buildManager->onRemoveUnit(unit);
	this->techManager->onRemoveUnit(unit);
	this->upgradeManager->onRemoveUnit(unit);
	this->workerManager->onRemoveUnit(unit);
	this->scoutManager->onRemoveUnit(unit);
	this->informationManager->onUnitDestroy(unit);
	this->armyManager->onUnitDestroy(unit);
	this->battleManager->onUnitDestroy(unit);
	this->defenseManager->onUnitDestroy(unit);
	this->expansionManager->onUnitDestroy(unit);
}

void BasicAIModule::onUnitShow(BWAPI::Unit* unit)
{
	this->informationManager->onUnitShow(unit);
	this->unitGroupManager->onUnitShow(unit);
	this->armyManager->onUnitShow(unit);
	this->defenseManager->onUnitShow(unit);
	this->expansionManager->onUnitShow(unit);
}
void BasicAIModule::onUnitHide(BWAPI::Unit* unit)
{
	this->informationManager->onUnitHide(unit);
	this->unitGroupManager->onUnitHide(unit);
	this->armyManager->onUnitHide(unit);
}

void BasicAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	this->unitGroupManager->onUnitMorph(unit);
}
void BasicAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
	this->unitGroupManager->onUnitRenegade(unit);
}

bool BasicAIModule::onSendText(std::string text)
{
	UnitType type=UnitTypes::getUnitType(text);
	
	if (text == "fast")
		Broodwar->setLocalSpeed(35);
	
	if (text == "very fast")
		Broodwar->setLocalSpeed(0);

	if (text == "slow")
		Broodwar->setLocalSpeed(70);

	if (text=="debug")
	{
		this->showManagerAssignments=true;
		this->buildOrderManager->enableDebugMode();
		this->scoutManager->enableDebugMode();
		return true;
	}
	if (text=="expand")
	{
		this->expansionManager->expand();
	}
	if (type!=UnitTypes::Unknown)
	{
		this->buildOrderManager->buildAdditional(1,type,300);
	}
	else {
		TechType type=TechTypes::getTechType(text);
		if (type!=TechTypes::Unknown)
		{
			this->techManager->research(type);
		}
		else
		{
			UpgradeType type=UpgradeTypes::getUpgradeType(text);
			if (type!=UpgradeTypes::Unknown)
			{
				this->upgradeManager->upgrade(type);
			}
			else
				Broodwar->printf("You typed '%s'!",text.c_str());
		}
	}
	return true;
}
