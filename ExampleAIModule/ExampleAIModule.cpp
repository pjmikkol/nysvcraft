#include <assert.h>
#include <cmath>
#include "ExampleAIModule.h"
#include <fstream>
#include <cstdlib>

#include <boost/bind.hpp>

#include <algorithm>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

#define NYSVCRAFT_START_FORMATION_X 400

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

bool unitIsLower(Unit* a, Unit* b) {
	return a->getPosition().y() > b->getPosition().y();
}

void ExampleAIModule::onStart()
{
	formed = false;
	dragoonDelay = 15;
	formationDelay = 100;

	initializeFleeThresholds();

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

	set<Unit*> units = Broodwar->self()->getUnits();

	foreach (Unit* unit, units) {				
		if (unit->getType() != UnitTypes::Protoss_Dragoon)			
			if (pastEnemy(unit))
				unit->rightClick(unit->getPosition() - Position(NYSVCRAFT_START_FORMATION_X + 200, 0));
			else
				unit->rightClick(unit->getPosition() + Position(NYSVCRAFT_START_FORMATION_X + 200, 0));

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

bool ExampleAIModule::pastEnemy(Unit* unit) {
	return unit->getPosition().x() >= Broodwar->mapWidth() * TILE_SIZE - Broodwar->self()->getStartLocation().x();
}

void ExampleAIModule::onEnd(bool isWinner)
{
	char* homep; 
	size_t len;
	_dupenv_s(&homep, &len, "USERPROFILE");
	string home(homep);

	string target = home + "\\Documents\\statistics.txt";

	string status = isWinner ? "Won" : "Lost";

	ofstream stats(target.c_str(), ios_base::app | ios_base::out);
	stats << status << "\t" << deadUnitCount() << "\t" << killedUnitCount() << endl;

	stats.close();
}

void ExampleAIModule::onFrame()
{
	if (dragoonDelay > 0)
		dragoonDelay--;

	if (formationDelay > 0)
		formationDelay--;

	drawUnitInfo();

	map< Unit*, set<Unit*> > * attackedBy = getAttackers();
	
	printAttackerInfo(attackedBy);
	decideActions(attackedBy);

	delete attackedBy;
}

void ExampleAIModule::printAttackerInfo(map<Unit*, set<Unit*> >* attackedBy) {
	pair< Unit*, set<Unit*> > p;
	
	foreach (p, *attackedBy) {	
		Position pos = p.first->getPosition();
		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%d", p.second.size());
	}
}

void ExampleAIModule::decideActions(map<Unit*, set<Unit*> >* attackedBy) {
	pair< Unit*, set<Unit*> > p;

	if (!formed && dragoonDelay == 0) {
		dragoonDelay = -1;

		int placeInLine = -4;

		set<Unit*> units = Broodwar->self()->getUnits();
		vector<Unit*> unitsV(units.begin(), units.end());
		sort(unitsV.begin(), unitsV.end(), unitIsLower);

		foreach (Unit* unit, unitsV)
			if (unit->getType() == UnitTypes::Protoss_Dragoon)
				if (unit->getPosition().x() >= Broodwar->mapWidth() * TILE_SIZE - Broodwar->self()->getStartLocation().x())
					unit->rightClick(Broodwar->self()->getStartLocation() - Position(NYSVCRAFT_START_FORMATION_X, 100 - placeInLine++ * 50));
				else
					unit->rightClick(Broodwar->self()->getStartLocation() + Position(NYSVCRAFT_START_FORMATION_X, 100 - placeInLine++ * 50));
	}

	if (!formed && formationDelay == 0) {
		formed = true;		
		Broodwar->printf("Formed");
	}

	if (formed && dragoonDelay <= 0 && Broodwar->enemy()->getUnits().empty()) {
		Broodwar->printf("Proceeding");

		dragoonDelay = 15;

		foreach (Unit* unit, Broodwar->self()->getUnits())
			if (unit->getPosition().x() >= Broodwar->mapWidth() * TILE_SIZE - Broodwar->self()->getStartLocation().x())
				unit->rightClick(unit->getPosition() - Position(100, 0));
			else
				unit->rightClick(unit->getPosition() + Position(100, 0));
	}

	foreach (p, *attackedBy) {			
		Unit* unit = p.first;

		handleFlee(unit, attackedBy);
		handleAttack(unit);
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
	set<Unit*> attackers = (*attackedBy)[unit];

	if (shouldFlee(unit, attackers)) {	
		Position runTo = fleeTo(unit, &attackers);
		
		data->state = flee;
		data->fleeCounter = getFleeDuration(unit, &attackers);

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
		} else {
			if (data->attackCounter > 0) {
				data->attackCounter--;
			} else if (!isInAttackRange(unit, unit->getOrderTarget())) {
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
			UnitType unitType = unit->getType();
			UnitType allyType = ally->getType();

			target = ally->getOrderTarget();
			if (!target) target = ally->getTarget();
		}
	}

	if (!target && !enemies.empty())
		target = weakestEnemyInRange(unit, enemies);

	if (!target && !enemies.empty())
		target = getClosestEnemy(unit, enemies);

	if (target) {
		unit->attackUnit(target);
		data->attackCounter = 10;
	}
}

Unit* ExampleAIModule::weakestEnemyInRange(Unit* unit, set<Unit*> enemies) {
	Unit* weakest = NULL;

	foreach (Unit* enemy, enemies) {
		if (!isInAttackRange(unit, enemy))
			continue;
		
		if (!weakest)
			weakest = enemy;
		else {
			if (weakest->getHitPoints() > enemy->getHitPoints())
				weakest = enemy;
		}
	}

	return weakest;
}

map< Unit*, set<Unit*> > * ExampleAIModule::getAttackers() {
	map< Unit*, set<Unit*> > * attackedBy = new map< Unit*, set<Unit*> >();

	set<Unit*> myUnits = Broodwar->self()->getUnits();

	foreach (Unit* unit, myUnits) {
		(*attackedBy)[unit] = set<Unit*>();
	}

	set<Unit*> enemyUnits = Broodwar->enemy()->getUnits();

	foreach (Unit* enemy, enemyUnits) {
		Unit* target = enemy->getOrderTarget();
		if (!target)
			target = enemy->getTarget();

		if (target && target->getPlayer() == Broodwar->self() && isInAttackRange(enemy, target))
			(*attackedBy)[target].insert(enemy);					
	}

	return attackedBy;
}

bool ExampleAIModule::isInAttackRange(Unit* attacker, Unit* target) {
	int attackerRange = attacker->getType().groundWeapon()->maxRange();

	if (attacker->getType() == UnitTypes::getUnitType(string("Protoss Dragoon")))
		// We assume enemy Dragoons have the Singularity Charge upgrade because API doesn't let us check enemy units' upgrade status.
		// We only do the upgrade check for allies.
		if (attacker->getPlayer() == Broodwar->enemy() || attacker->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
			attackerRange = attackerRange * 6 / 4;

	return attacker->getDistance(target) <= attackerRange;
}	

void ExampleAIModule::drawUnitInfo()
{
	foreach (Unit* unit, Broodwar->self()->getUnits()) {
		Position pos = unit->getPosition();
		Unit* targetUnit = unit->getOrderTarget();
		Position target = unit->getTargetPosition();
		Color color;

		color = Color(targetUnit ? Colors::Red : Colors::Green);
			
		if (unit->getType() == UnitTypes::Protoss_Dragoon)
			Broodwar->drawLineMap(pos.x(), pos.y(), target.x(), target.y(), color);

		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 16, "\x05(%d, %d)", pos.x(), pos.y());

		//Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%s", stateName(getUnitData(*i).state).c_str());
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
