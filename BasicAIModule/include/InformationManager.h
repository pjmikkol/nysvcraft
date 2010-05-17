#pragma once
#include <BWAPI.h>
#include <map>
class InformationManager
{
  public:
    InformationManager();
    void onUnitShow(BWAPI::Unit* unit);
    void onUnitHide(BWAPI::Unit* unit);
    void onUnitDestroy(BWAPI::Unit* unit);
    BWAPI::Player* getPlayer(BWAPI::Unit* unit);
    BWAPI::UnitType getType(BWAPI::Unit* unit);
    BWAPI::Position getLastPosition(BWAPI::Unit* unit);
    int getLastSeenTime(BWAPI::Unit* unit);
    bool exists(BWAPI::Unit* unit);
    bool enemyHasBuilt(BWAPI::UnitType type);
    int getBuildTime(BWAPI::UnitType type);
  private:
    class UnitData
    {
      public:
        UnitData();
        BWAPI::Position position;
        BWAPI::UnitType type;
        BWAPI::Player* player;
        int lastSeenTime;
        bool exists;
    };
    void updateBuildTime(BWAPI::UnitType type, int time);
    std::map<BWAPI::Unit*, UnitData> savedData;
    std::map<BWAPI::UnitType, int> buildTime;

};