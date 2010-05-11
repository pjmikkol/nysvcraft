#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>

using namespace std;
using namespace BWAPI;

enum State { flee, default_state, fight };


struct UnitData {
	State state;
	int fleeCounter;
	int group; // Which group the unit belongs?
};

struct GroupInfo { 
	Unit* leader; // "Leader" this group. The group moves according to this guy
};

class ExampleAIModule : public AIModule
{
public:
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual bool onSendText(string text);
	virtual void onPlayerLeft(Player* player);
	virtual void onNukeDetect(Position target);
	virtual void onUnitCreate(Unit* unit);
	virtual void onUnitDestroy(Unit* unit);
	virtual void onUnitMorph(Unit* unit);
	virtual void onUnitShow(Unit* unit);
	virtual void onUnitHide(Unit* unit);
	virtual void onUnitRenegade(Unit* unit);
private:
	Position center;
	map< int, GroupInfo > groupData;
	void drawUnitInfo();
	map< Unit*, int > * ExampleAIModule::getAttackerCount();
	map< Unit*, UnitData > unitData;
	UnitData getUnitData(Unit* unit);
	bool ExampleAIModule::isAttackingEnemy(Unit* unit);
	void ExampleAIModule::printAttackerInfo(map<Unit*, int>* attacking);
	void ExampleAIModule::decideActions(map<Unit*, int>* attacking);
	void ExampleAIModule::handleFlee(Unit* unit, map<Unit*, int>* attacking);
	void ExampleAIModule::handleAttack(Unit* unit);
	Unit* ExampleAIModule::getClosestEnemy(Unit* unit, set<Unit*> enemies);
	Unit* ExampleAIModule::getClosestUnitFrom(Position &pos, set<Unit*> units);
	bool ExampleAIModule::isInAttackRange(Unit* attacker, Unit* target);
};