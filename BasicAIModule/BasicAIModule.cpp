#include "BasicAIModule.h"
using namespace BWAPI;

DWORD WINAPI AnalyzeThread() {
	BWTA::readMap();
	BWTA::analyze();

	analyzed = true;

	return 0;
}

void BasicAIModule::onStart()
{
	analyzed = false;

	this->showManagerAssignments = false;

	if (Broodwar->isReplay()) return;

	Broodwar->enableFlag(Flag::UserInput);

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) AnalyzeThread, NULL, 0, NULL);

	this->buildManager       = new BuildManager(&this->arbitrator);
	this->techManager        = new TechManager(&this->arbitrator);
	this->upgradeManager     = new UpgradeManager(&this->arbitrator);
	this->scoutManager       = new ScoutManager(&this->arbitrator);
	this->workerManager      = new WorkerManager(&this->arbitrator);
	this->buildOrderManager  = new BuildOrderManager(this->buildManager,this->techManager,this->upgradeManager,this->workerManager);
	this->baseManager        = new BaseManager();
	this->supplyManager      = new SupplyManager();
	this->defenseManager     = new DefenseManager(&this->arbitrator);
	this->informationManager = new InformationManager();
	this->unitGroupManager   = new UnitGroupManager();
	this->enhancedUI         = new EnhancedUI();

	this->supplyManager->setBuildManager(this->buildManager);
	this->supplyManager->setBuildOrderManager(this->buildOrderManager);
	this->techManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
	this->upgradeManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
	this->workerManager->setBaseManager(this->baseManager);
	this->workerManager->setBuildOrderManager(this->buildOrderManager);
	this->baseManager->setBuildOrderManager(this->buildOrderManager);

	this->buildOrderManager->enableDependencyResolver();
	this->workerManager->enableAutoBuild();
	this->workerManager->setAutoBuildPriority(40); 
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
	this->defenseManager->onRemoveUnit(unit);
	this->informationManager->onUnitDestroy(unit);
}

void BasicAIModule::onUnitShow(BWAPI::Unit* unit)
{
	this->informationManager->onUnitShow(unit);
	this->unitGroupManager->onUnitShow(unit);
}
void BasicAIModule::onUnitHide(BWAPI::Unit* unit)
{
	this->informationManager->onUnitHide(unit);
	this->unitGroupManager->onUnitHide(unit);
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
	if (text=="debug")
	{
		this->showManagerAssignments=true;
		this->buildOrderManager->enableDebugMode();
		this->scoutManager->enableDebugMode();
		return true;
	}
	if (text=="expand")
	{
		this->baseManager->expand();
	}
	if (type!=UnitTypes::Unknown)
	{
		this->buildOrderManager->buildAdditional(1,type,300);
	}
	else
	{
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
