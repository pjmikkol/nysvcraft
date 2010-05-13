#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include "Group.h"
#include "Formation.h"
#include "Helpers.h"

using namespace std;
using namespace BWAPI;


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

	map< Unit*, set<Unit*> > * getAttackers();
	void drawUnitInfo();
	UnitData getUnitData(Unit* unit);
	void printAttackerInfo(map<Unit*, set<Unit*> >* attacking);
	void decideActions(map<Unit*, set<Unit*> >* attacking);
	void handleFlee(Unit* unit, map<Unit*, set<Unit*> >* attacking);
	void handleAttack(Unit* unit);
	bool isInAttackRange(Unit* attacker, Unit* target);
	Unit* weakestEnemyInRange(Unit* unit, set<Unit*> enemies);
	void calculateTarget(Unit* unit, set<Unit*> enemies);

	Position fleeTo(Unit* unit, const set<Unit*>* attackers);
	set<double>* calculateAngles(Unit* unit, const set<Unit*>* attackers);
	double calculateAngle(Unit* unit, Unit* enemy);
	double midAngle(set<double>* angles);
	double reverseAngle(double angle);

	//Ugly test
	Group* g;
};
