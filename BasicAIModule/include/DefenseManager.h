#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "BuildOrderManager.h"
#include "BaseManager.h"
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
  void onRemoveUnit(BWAPI::Unit* unit);
  virtual void update();
  virtual std::string getName() const;
  virtual std::string getShortName() const;
  std::set<BWTA::Chokepoint*> findInterestingChokepoints();

private:
  BuildOrderManager* buildOrderManager;
  BaseManager* baseManager;
  Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
  std::map<BWAPI::Unit*,DefenseData> defenders;
  std::set<BWTA::Chokepoint*> interestingChokepoints;
};