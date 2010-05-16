#include "BasicAIModule.h"
using namespace BWAPI;

void BasicAIModule::onStart()
{
  this->showManagerAssignments=false;
  if (Broodwar->isReplay()) return;
  // Enable some cheat flags
  Broodwar->enableFlag(Flag::UserInput);
  //Broodwar->enableFlag(Flag::CompleteMapInformation);
  BWTA::readMap();
  BWTA::analyze();
  this->analyzed=true;
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
  
  BWAPI::Race race = Broodwar->self()->getRace();
  BWAPI::Race enemyRace = Broodwar->enemy()->getRace();
  BWAPI::UnitType workerType=*(race.getWorker());
    double minDist;
  BWTA::BaseLocation* natural=NULL;
  BWTA::BaseLocation* home=BWTA::getStartLocation(Broodwar->self());
  for(std::set<BWTA::BaseLocation*>::const_iterator b=BWTA::getBaseLocations().begin();b!=BWTA::getBaseLocations().end();b++)
  {
    if (*b==home) continue;
    double dist=home->getGroundDistance(*b);
    if (dist>0)
    {
      if (natural==NULL || dist<minDist)
      {
        minDist=dist;
        natural=*b;
      }
    }
  }
  this->buildOrderManager->enableDependencyResolver();
  //make the basic production facility
  if (race == Races::Zerg)
  {
    //send an overlord out if Zerg
    this->scoutManager->setScoutCount(1);

    //12 hatch
    this->buildOrderManager->build(12,workerType,80);
    this->baseManager->expand(natural,79);
    this->buildOrderManager->build(20,workerType,78);
    this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,60);
    this->buildOrderManager->buildAdditional(3,UnitTypes::Zerg_Zergling,82);
  }
  else if (race == Races::Terran)
  {
    this->buildOrderManager->build(20,workerType,80);
    if (enemyRace == Races::Zerg)
    {
      this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Barracks,60);
      this->buildOrderManager->buildAdditional(9,UnitTypes::Terran_Marine,45);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Refinery,42);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Barracks,40);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Terran_Academy,39);
      this->buildOrderManager->buildAdditional(9,UnitTypes::Terran_Medic,38);
      this->buildOrderManager->research(TechTypes::Stim_Packs,35);
      this->buildOrderManager->research(TechTypes::Tank_Siege_Mode,35);
      this->buildOrderManager->buildAdditional(3,UnitTypes::Terran_Siege_Tank_Tank_Mode,34);
      this->buildOrderManager->buildAdditional(2,UnitTypes::Terran_Science_Vessel,30);
      this->buildOrderManager->research(TechTypes::Irradiate,30);
      this->buildOrderManager->upgrade(1,UpgradeTypes::Terran_Infantry_Weapons,20);
      this->buildOrderManager->build(3,UnitTypes::Terran_Missile_Turret,13);
      this->buildOrderManager->upgrade(3,UpgradeTypes::Terran_Infantry_Weapons,12);
      this->buildOrderManager->upgrade(3,UpgradeTypes::Terran_Infantry_Armor,12);
      this->buildOrderManager->build(1,UnitTypes::Terran_Engineering_Bay,11);
      this->buildOrderManager->buildAdditional(40,UnitTypes::Terran_Marine,10);
      this->buildOrderManager->build(6,UnitTypes::Terran_Barracks,8);
      this->buildOrderManager->build(2,UnitTypes::Terran_Engineering_Bay,7);
      this->buildOrderManager->buildAdditional(10,UnitTypes::Terran_Siege_Tank_Tank_Mode,5);
    }
    else
    {
      this->buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Terran_Machine_Shop,70);
      this->buildOrderManager->buildAdditional(3,BWAPI::UnitTypes::Terran_Factory,60);
      this->buildOrderManager->research(TechTypes::Spider_Mines,55);
      this->buildOrderManager->research(TechTypes::Tank_Siege_Mode,55);
      this->buildOrderManager->buildAdditional(20,BWAPI::UnitTypes::Terran_Vulture,40);
      this->buildOrderManager->buildAdditional(20,BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode,40);
      this->buildOrderManager->upgrade(3,UpgradeTypes::Terran_Vehicle_Weapons,20);
    }
  }
  else if (race == Races::Protoss)
  {
    this->buildOrderManager->build(20,workerType,80);
    this->buildOrderManager->buildAdditional(40,UnitTypes::Protoss_Dragoon,70);
    this->buildOrderManager->buildAdditional(40,UnitTypes::Protoss_Zealot,70);
	this->buildOrderManager->buildAdditional(5, UnitTypes::Protoss_Photon_Cannon, 70);
    this->buildOrderManager->upgrade(1,UpgradeTypes::Singularity_Charge,61);
    this->buildOrderManager->buildAdditional(20,UnitTypes::Protoss_Carrier,60);
  }
  this->workerManager->enableAutoBuild();
  this->workerManager->setAutoBuildPriority(40);
 
}


void BasicAIModule::onFrame()
{
  if (Broodwar->isReplay()) return;
  if (!this->analyzed) return;
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

  if (Broodwar->getFrameCount()>24*50)
    scoutManager->setScoutCount(1);

  if ((float) this->buildManager->getCompletedCount(UnitTypes::Protoss_Probe) / this->baseManager->getAllBases().size() > 10) {
	  this->baseManager->expand();
  }

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
