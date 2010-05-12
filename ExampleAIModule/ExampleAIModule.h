#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Group.h"
#include "Formation.h"
#include "Helpers.h"

using namespace std;
using namespace BWAPI;

/*States are:
	flee			- for running away from enemy
	fight			- kicking ass
	formation		- when moving in formation finding enemy or doing something
*/
enum State { flee, fight, formation };


struct UnitData {
	State state;
	int fleeCounter;
	int group; // Which group the unit belongs?
	int attackCounter;
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
	map< int, Group> groupData;
	map< Unit*, UnitData > unitData;

	map< Unit*, set<Unit*> > * getAttackerCount();
	void drawUnitInfo();
	UnitData getUnitData(Unit* unit);
	bool isAttackingEnemy(Unit* unit);
	void printAttackerInfo(map<Unit*, set<Unit*> >* attacking);
	void decideActions(map<Unit*, set<Unit*> >* attacking);
	void handleFlee(Unit* unit, map<Unit*, set<Unit*> >* attacking);
	void handleAttack(Unit* unit);
	bool isInAttackRange(Unit* attacker, Unit* target);
	Unit* getClosestEnemy(Unit* unit, set<Unit*> enemies);
	Unit* weakestEnemyInRange(Unit* unit, set<Unit*> enemies);
	void calculateTarget(Unit* unit, set<Unit*> enemies);
};
