#include <InformationManager.h>
#include "Util.h"
InformationManager::InformationManager()
{
  buildTime[*BWAPI::Broodwar->enemy()->getRace().getCenter()]=0;
  buildTime[*BWAPI::Broodwar->enemy()->getRace().getWorker()]=0;
  if (BWAPI::Broodwar->enemy()->getRace()==BWAPI::Races::Zerg)
  {
    buildTime[BWAPI::UnitTypes::Zerg_Larva]=0;
    buildTime[BWAPI::UnitTypes::Zerg_Overlord]=0;
  }
}
void InformationManager::onUnitShow(BWAPI::Unit* unit)
{
  savedData[unit].exists=true;
  if (!BWAPI::Broodwar->self()->isEnemy(unit->getPlayer())) return;
  int time=BWAPI::Broodwar->getFrameCount();
  BWAPI::UnitType type=unit->getType();
  updateBuildTime(type,time-type.buildTime());  
}
void InformationManager::onUnitHide(BWAPI::Unit* unit)
{
  savedData[unit].player=unit->getPlayer();
  savedData[unit].type=unit->getType();
  savedData[unit].position=unit->getPosition();
  savedData[unit].lastSeenTime=BWAPI::Broodwar->getFrameCount();
}
void InformationManager::onUnitDestroy(BWAPI::Unit* unit)
{
  savedData[unit].player=unit->getPlayer();
  savedData[unit].type=unit->getType();
  savedData[unit].position=unit->getPosition();
  savedData[unit].lastSeenTime=BWAPI::Broodwar->getFrameCount();
  savedData[unit].exists=false;
}

BWAPI::Player* InformationManager::getPlayer(BWAPI::Unit* unit)
{
  if (unit->exists())
    return unit->getPlayer();
  return savedData[unit].player;
}

BWAPI::UnitType InformationManager::getType(BWAPI::Unit* unit)
{
  if (unit->exists())
    return unit->getType();
  return savedData[unit].type;
}

BWAPI::Position InformationManager::getLastPosition(BWAPI::Unit* unit)
{
  if (unit->exists())
    return unit->getPosition();
  return savedData[unit].position;
}

int InformationManager::getLastSeenTime(BWAPI::Unit* unit)
{
  if (unit->exists())
    return BWAPI::Broodwar->getFrameCount();
  return savedData[unit].lastSeenTime;
}

bool InformationManager::exists(BWAPI::Unit* unit)
{
  if (unit->exists())
    return true;
  return savedData[unit].exists;
}

bool InformationManager::enemyHasBuilt(BWAPI::UnitType type)
{
  return (buildTime.find(type)!=buildTime.end());
}
int InformationManager::getBuildTime(BWAPI::UnitType type)
{
  std::map<BWAPI::UnitType, int>::iterator i=buildTime.find(type);
  if (i==buildTime.end())
    return -1;
  return i->second;
}

void InformationManager::updateBuildTime(BWAPI::UnitType type, int time)
{
  std::map<BWAPI::UnitType, int>::iterator i=buildTime.find(type);
  if (i!=buildTime.end() && (i->second<=time || i->second==0)) return;
  buildTime[type]=time;
  if (time<0) return;
  for(std::map<const BWAPI::UnitType*,int>::const_iterator i=type.requiredUnits().begin();i!=type.requiredUnits().end();i++)
  {
    updateBuildTime(*i->first,time-i->first->buildTime());
  }
}

InformationManager::UnitData::UnitData()
{
  position=BWAPI::Positions::Unknown;
  type=BWAPI::UnitTypes::Unknown;
  player=NULL;
}