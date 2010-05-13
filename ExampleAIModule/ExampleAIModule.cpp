#include <assert.h>
#include <cmath>
#include "ExampleAIModule.h"

using namespace BWAPI;
using namespace std;
using namespace helpers;

const double pi = 3.14159;

string stateName(State state) {
	switch (state) {
		case flee:
			return "flee";
		case fight:
			return "fight";
		case formation:
			return "formation";
		default:
			return "penis";
	}
}

void ExampleAIModule::onStart()
{
	Broodwar->setLocalSpeed(50);
	Broodwar->printf("The map is %s, a %d player map",Broodwar->mapName().c_str(),Broodwar->getStartLocations().size());
	// Enable some cheat flags
	Broodwar->enableFlag(Flag::UserInput);

	this->center = Position((Broodwar->mapWidth() * TILE_SIZE)/2 , (Broodwar->mapHeight() * TILE_SIZE)/2 );
	this->unitData = map< Unit*, UnitData >();
	this->groupData = map<int, Group>();

	// Calculate the center of group nro 1 in this loop,
	// In this version always start with single group.
	Position groupCenter = Position(0, 0);
	Group startGroup = Group(1, &this->unitData);
	for(set<Unit*>::const_iterator i = Broodwar->self()->getUnits().begin();
	    i != Broodwar->self()->getUnits().end();
	    i++) {
		
		Unit* unit = *i;
		// Do something clever here, formation?
		unit->attackMove(this->center);  
		UnitData unitData;
		unitData.state = fight;
		unitData.fleeCounter = 0;
		unitData.group = 1;
		unitData.attackCounter = 0;
		this->unitData.insert(make_pair(unit, unitData));

		startGroup.add(unit);
		groupCenter += unit->getPosition();
		Broodwar->printf("Initial hit points: %d", unit->getType().maxHitPoints());
	}
	int unitsInGroup = startGroup.getSize();
	this->groupData.insert(make_pair(startGroup.getId(), startGroup));
	/* TODO: Group AI initialization */
	groupCenter = Position(groupCenter.x()/unitsInGroup, groupCenter.y()/unitsInGroup);
	Unit* boss = getClosestUnitFrom(groupCenter, Broodwar->self()->getUnits());
	Broodwar->printf("Size of the group 1: %d", unitsInGroup);
	/* Testing that the functions work correctly: */
	Group g = this->groupData.find(1)->second;
	g.setFormation(parabola);
	g.remove(boss);
	Broodwar->printf("Size of the group 1: %d", g.getSize() );
}

void ExampleAIModule::onEnd(bool isWinner)
{
  if (isWinner)
  {
    //log win to file
  }
}

void ExampleAIModule::onFrame()
{
	drawUnitInfo();

	map< Unit*, set<Unit*> > * attackedBy = this->getAttackers();
	
	this->printAttackerInfo(attackedBy);
	this->decideActions(attackedBy);

	delete attackedBy;
}

void ExampleAIModule::printAttackerInfo(map<Unit*, set<Unit*> >* attackedBy) {
	for (map<Unit*, set<Unit*> >::const_iterator iter = attackedBy->begin(); iter != attackedBy->end(); iter++) {
		Position pos = iter->first->getPosition();
		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%d", iter->second.size());
	}
}

void ExampleAIModule::decideActions(map<Unit*, set<Unit*> >* attackedBy) {
	for(set<Unit*>::const_iterator u = Broodwar->getAllUnits().begin(); u != Broodwar->getAllUnits().end(); u++) {
		Unit* unit = *u;

		this->handleFlee(unit, attackedBy);
		this->handleAttack(unit);
	}
}

void ExampleAIModule::handleFlee(Unit* unit, map<Unit*, set<Unit*> >* attackedBy) {
	UnitData* data = &unitData[unit];

	if (data->state == flee) {
		if (data->fleeCounter > 0) {
			data->fleeCounter--;
			return;
		}
		else {
			data->state = fight;
		}
	}

	// TODO add smarter flee
	// Parameterize on unit type
	if ((*attackedBy)[unit].size() > 1) {
		TilePosition runTo = fleeTo(unit, &(*attackedBy)[unit]);
		
		data->state = flee;
		data->fleeCounter = 25;

		unit->rightClick(runTo);
	}
}

Position ExampleAIModule::fleeTo(Unit* unit, const set<Unit*>* attackers) {
	assert(attackers->size() > 0);

	set<double>* angles = calculateAngles(unit, attackers);
	double mid = midAngle(angles);
	Position direction = vecFromAngle(reverseAngle(mid), 2);

	return unit->getPosition() + direction;
}

set<double>* ExampleAIModule::calculateAngles(Unit* unit, const set<Unit*>* attackers) {
	set<double>* angles = new set<double>();
	for (set<Unit*>::const_iterator iter = attackers->begin(); iter != attackers->end(); iter++) {
		Unit* enemy = *iter;
		angles->insert(calculateAngle(unit, enemy));
	}
	return angles;
}

double ExampleAIModule::calculateAngle(Unit* unit, Unit* enemy) {
	Position enemyVec = enemy->getPosition() - unit->getPosition();
	Position neutralVec = Position(1, 0);
	return angleBetween(enemyVec, neutralVec);
}
	

double ExampleAIModule::midAngle(set<double>* angles) {
	double sum = 0;
	for (set<double>::const_iterator iter = angles->begin(); iter != angles->end(); iter++) {
		sum += *iter;
	}
	return sum / angles->size();
}

double ExampleAIModule::reverseAngle(double angle) {
	double reverse = angle - pi;
	if (reverse < 0) 
		reverse += 2*pi;
	return reverse;
}

void ExampleAIModule::handleAttack(Unit* unit) {
	UnitData* data = &unitData[unit];

	if (data->state == fight) {
		set<Unit*> enemies = Broodwar->enemy()->getUnits();
		if (!isAttackingEnemy(unit)) {
			calculateTarget(unit, enemies);
		}
		else {
			if (data->attackCounter > 0) {
				data->attackCounter--;
			} else {
				calculateTarget(unit, enemies);
			}
		}
	}
	else {
		//TODO: figure out where to move
	}	
}

void ExampleAIModule::calculateTarget(Unit* unit, set<Unit*> enemies) {
	UnitData* data = &unitData[unit];
	Unit* target = NULL;

	if (4 * unit->getHitPoints() < unit->getType().maxHitPoints()) {
		set<Unit*> attackingAllies = getAttackingAllies();
		Unit* ally = getClosestUnitFrom(unit->getPosition(), attackingAllies);
		if (ally) {
			target = ally->getOrderTarget();
		}
	}
	else {
		target = weakestEnemyInRange(unit, enemies);
	}

	if (!target && !enemies.empty())
		target = getClosestEnemy(unit, enemies);
	if (target) {
		unit->attackUnit(target);
		data->attackCounter = 50;
	}
}

Unit* ExampleAIModule::weakestEnemyInRange(Unit* unit, set<Unit*> enemies) {
	Unit* weakest = NULL;
	for (set<Unit*>::const_iterator iter = enemies.begin(); iter != enemies.end(); iter++) {
		Unit* current = *iter;
		if (!isInAttackRange(current, unit))
			continue;
		
		if (!weakest)
			weakest = current;
		else {
			if (weakest->getHitPoints() > current->getHitPoints())
				weakest = current;
		}
	}
	return weakest;
}

map< Unit*, set<Unit*> > * ExampleAIModule::getAttackers() {
	map< Unit*, set<Unit*> > * attackedBy = new map< Unit*, set<Unit*> >();

	set<Unit*> myUnits = Broodwar->self()->getUnits();

	for (set<Unit*>::const_iterator iter = myUnits.begin(); iter != myUnits.end(); iter++) {
		Unit* u = *iter;
		(*attackedBy)[u] = set<Unit*>();
	}

	set<Unit*> enemyUnits = Broodwar->enemy()->getUnits();

	for(set<Unit*>::const_iterator iter = enemyUnits.begin(); iter != enemyUnits.end(); iter++) {
		Unit* enemy = *iter;							
		Unit* target = enemy->getOrderTarget();

		if (target && target->getPlayer() == Broodwar->self() && this->isInAttackRange(enemy, target))
			(*attackedBy)[target].insert(enemy);					
	}
	return attackedBy;
}

bool ExampleAIModule::isInAttackRange(Unit* attacker, Unit* target) {
	return attacker->getDistance(target) <= attacker->getType().groundWeapon()->maxRange();
}	

void ExampleAIModule::drawUnitInfo()
{
	for(set<Unit*>::const_iterator i=Broodwar->self()->getUnits().begin();i!=Broodwar->self()->getUnits().end();i++)
	{
		Position pos = (*i)->getPosition();
		Unit* targetUnit = (*i)->getOrderTarget();
		Position target = (*i)->getTargetPosition();
		Color color;

		if (targetUnit)
			color = Color(Colors::Red);
		else
			color = Color(Colors::Green);
			
		Broodwar->drawLineMap(pos.x(), pos.y(), target.x(), target.y(), color);

		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 16, "\x05(%d, %d)", pos.x(), pos.y());

		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%s", stateName(this->getUnitData(*i).state).c_str());
  }
}

UnitData ExampleAIModule::getUnitData(Unit* unit) {
	return this->unitData.find(unit)->second;
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitMorph(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitShow(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitHide(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit* unit)
{

}

void ExampleAIModule::onPlayerLeft(BWAPI::Player* player)
{

}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{

}

bool ExampleAIModule::onSendText(string text)
{
  if (text=="/show players")
  {
    return false;
  }
  return true;
}
