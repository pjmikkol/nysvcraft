#include <BaseManager.h>
BaseManager::BaseManager()
{
  this->builder = NULL;
}
void BaseManager::setBuildOrderManager(BuildOrderManager* builder)
{
  this->builder = builder;
}

void BaseManager::update()
{
  for(std::set<Base*>::iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
  {
    if (!(*b)->isActive())
    {
      if ((*b)->getResourceDepot() == NULL)
      {
        BWAPI::TilePosition tile = (*b)->getBaseLocation()->getTilePosition();
        std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(tile.x(),tile.y());
        for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
          if ((*u)->getPlayer() == BWAPI::Broodwar->self() && (*u)->getType().isResourceDepot())
          {
            (*b)->setResourceDepot(*u);
            break;
          }
      }
      if ((*b)->getResourceDepot()!=NULL)
      {
        if ((*b)->getResourceDepot()->exists()==false)
          (*b)->setResourceDepot(NULL);
        else
        {
          if ((*b)->getResourceDepot()->isCompleted() || (*b)->getResourceDepot()->getRemainingBuildTime() < 250)
            (*b)->setActive(true);
        }
      }
    }
  }

  //check to see if any new base locations need to be added
  for(std::set<BWTA::BaseLocation*>::const_iterator bl = BWTA::getBaseLocations().begin(); bl != BWTA::getBaseLocations().end(); bl++)
    if (location2base.find(*bl) == location2base.end())
    {
      BWAPI::TilePosition tile = (*bl)->getTilePosition();
      std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(tile.x(), tile.y());
      for(std::set<BWAPI::Unit*>::iterator u = units.begin(); u != units.end(); u++)
        if ((*u)->getPlayer() == BWAPI::Broodwar->self() && (*u)->getType().isResourceDepot())
          addBase(*bl);
    }
}

void BaseManager::addBase(BWTA::BaseLocation* location)
{
  Base* newBase = new Base(location);
  newBase->setBeingConstructed(true);
  allBases.insert(newBase);
  this->location2base[location] = newBase;
}
Base* BaseManager::getBase(BWTA::BaseLocation* location)
{
  std::map<BWTA::BaseLocation*,Base*>::iterator i=location2base.find(location);
  if (i==location2base.end())
    return NULL;
  return i->second;
}


Base* BaseManager::expand(int priority)
{
  BWTA::BaseLocation* location=NULL;
  double minDist=-1;
  BWTA::BaseLocation* home=BWTA::getStartLocation(BWAPI::Broodwar->self());
  for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
  {
    double dist=home->getGroundDistance(*i);
    if (dist>0 && getBase(*i)==NULL)
    {
      if (minDist == -1 || dist<minDist)
      {
        minDist=dist;
        location=*i;
      }
    }
  }
  return expand(location,priority);
}
Base* BaseManager::expand(BWTA::BaseLocation* location, int priority)
{
  if (location==NULL)
    return NULL; //FIXME: keep this in mind
  addBase(location);
  this->builder->buildAdditional(1,*BWAPI::Broodwar->self()->getRace().getCenter(),priority,location->getTilePosition());
  return getBase(location);
}


std::set<Base*> BaseManager::getActiveBases() const
{
  std::set<Base*> activeBases;
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
    if ((*b)->isActive())
      activeBases.insert(*b);
  return activeBases;
}
std::set<Base*> BaseManager::getAllBases() const
{
  std::set<Base*> allBases;
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
    allBases.insert(*b);
  return allBases;
}
std::string BaseManager::getName()
{
  return "Base Manager";
}
void BaseManager::onRemoveUnit(BWAPI::Unit* unit)
{
  for(std::set<Base*>::const_iterator b = this->allBases.begin(); b != this->allBases.end(); b++)
  {
    if((*b)->getResourceDepot() == unit)
    {
      if (unit->isCompleted())
        allBases.erase(b);
      else
        (*b)->setResourceDepot(NULL);
    }
  }
}