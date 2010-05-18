#pragma once
#include <Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
#include "BuildOrderManager.h"

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
  DefenseManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator, BuildOrderManager* buildOrderManager);
  virtual void onOffer(std::set<BWAPI::Unit*> units);
  virtual void onRevoke(BWAPI::Unit* unit, double bid);
  void onRemoveUnit(BWAPI::Unit* unit);
  virtual void update();
  virtual std::string getName() const;
  virtual std::string getShortName() const;

private:
  BuildOrderManager* buildOrderManager;
  Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator;
  BWAPI::Position chokePosition;
  std::map<BWAPI::Unit*,DefenseData> defenders;
};