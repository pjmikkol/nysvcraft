#include "BattleManager.h"

const double releaseDist = 300;

BattleManager::BattleManager(Arbitrator::Arbitrator<BWAPI::Unit*, double>* arbitrator)
{
	this->arbitrator = arbitrator;
	this->fighters = new map<Unit*, UnitData>();
}

BattleManager::~BattleManager(void)
{
	delete this->fighters;
}

void BattleManager::onOffer(set<Unit*> units)
{
	foreach (Unit* unit, units) {
		if (unit->getType() == UnitTypes::Protoss_Zealot || unit->getType() == UnitTypes::Protoss_Dragoon) {
			if (this->arbitrator->accept(this, unit)) {
				UnitData ud = { fight, 0, 0 };
				this->fighters->insert(make_pair(unit, ud));
			} else this->fighters->erase(unit);
		}
	}
}

void BattleManager::onRevoke(Unit* unit, double bid)
{
	//this->arbitrator->setBid(this, unit, 99999);
}

void BattleManager::BidUnits()
{
	foreach (Unit* unit, Broodwar->self()->getUnits()) {
		if ( doWeWantUnit(unit) ) {
			this->arbitrator->setBid(this, unit, 300);
		} else {
			this->arbitrator->removeBid(this, unit);
		}
	}
}

bool BattleManager::doWeWantUnit(Unit* unit)
{
	if (!unit) return false;

	//TODO: fix this hard-coded number
	const double maxDist = 150;

	UnitType t = unit->getType();
	if ( t == UnitTypes::Protoss_Zealot || t == UnitTypes::Protoss_Dragoon) {
		Unit* enemy = getClosestEnemy(unit, Broodwar->enemy()->getUnits());

		if (enemy && enemy->getPosition().getDistance(unit->getPosition()) < maxDist)
			return true;
	}
	return false;
}

void BattleManager::update()
{
	BidUnits();
	/*Micro-level attack code goes here*/
	//drawUnitInfo();
	map< Unit*, set<Unit*> > * attackedBy = getAttackers();
	
	//printAttackerInfo(attackedBy);
	decideActions(attackedBy);

	delete attackedBy;
}

void BattleManager::onUnitShow(Unit* unit)
{

}

void BattleManager::onUnitHide(Unit* unit)
{
	//setUnitsFree();
}

void BattleManager::setUnitsFree()
{
	set<Unit*> toReleased;
	pair<Unit*, UnitData> p;
	foreach(p, *this->fighters) {
		if (canWeReleaseUnit(p.first)) {
			this->arbitrator->removeBid(this, p.first);
			toReleased.insert(p.first);
		}
	}
	foreach(Unit* u, toReleased) {
		this->fighters->erase(u);
	}
}

bool BattleManager::canWeReleaseUnit(Unit* u) {
	Unit* enemy = getClosestEnemy(u, Broodwar->self()->getUnits());
	if (enemy && enemy->getPosition().getDistance(u->getPosition()) < releaseDist)
		return false;
	return true;
}

void BattleManager::onUnitDestroy(Unit* unit)
{
	this->fighters->erase(unit);
	//setUnitsFree();
}

void BattleManager::onRemoveUnit(Unit* unit)
{
	this->fighters->erase(unit);
}


string BattleManager::getName() const
{
	return "Battle Manager";
}

string BattleManager::getShortName() const
{
	return "Battle";
}


void BattleManager::printAttackerInfo(map<Unit*, set<Unit*> >* attackedBy) {
	pair< Unit*, set<Unit*> > p;
	
	foreach (p, *attackedBy) {	
		Position pos = p.first->getPosition();
		Broodwar->drawTextMap(pos.x() - 16, pos.y() - 26, "%d", p.second.size());
	}
}

void BattleManager::decideActions(map<Unit*, set<Unit*> >* attackedBy) {
	pair< Unit*, set<Unit*> > p;

	foreach (p, *attackedBy) {			
		Unit* unit = p.first;

		handleFlee(unit, attackedBy);
		handleAttack(unit);
	}
}


void BattleManager::handleFlee(Unit* unit, map<Unit*, set<Unit*> >* attackedBy) {
	UnitData* data = &((*fighters)[unit]);

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

Position BattleManager::fleeTo(Unit* unit, const set<Unit*>* attackers) {
	assert(attackers->size() > 0);

	set<double>* angles = calculateAngles(unit, attackers);
	double mid = midAngle(angles);
	Position direction = vecFromAngle(reverseAngle(mid), 2);

	return unit->getPosition() + direction;
}

set<double>* BattleManager::calculateAngles(Unit* unit, const set<Unit*>* attackers) {
	set<double>* angles = new set<double>();
	for (set<Unit*>::const_iterator iter = attackers->begin(); iter != attackers->end(); ++iter) {
		Unit* enemy = *iter;
		angles->insert(calculateAngle(unit, enemy));
	}
	return angles;
}

double BattleManager::calculateAngle(Unit* unit, Unit* enemy) {
	Position enemyVec = enemy->getPosition() - unit->getPosition();
	Position neutralVec = Position(1, 0);
	return angleBetween(enemyVec, neutralVec);
}
	

double BattleManager::midAngle(set<double>* angles) {
	double sum = 0;
	for (set<double>::const_iterator iter = angles->begin(); iter != angles->end(); iter++) {
		sum += *iter;
	}
	return sum / angles->size();
}

double BattleManager::reverseAngle(double angle) {
	double reverse = angle - pi;
	if (reverse < 0) 
		reverse += 2*pi;
	return reverse;
}

void BattleManager::handleAttack(Unit* unit) {
	UnitData* data = &((*fighters)[unit]);

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

void BattleManager::calculateTarget(Unit* unit, set<Unit*> enemies) {
	UnitData* data = &((*fighters)[unit]);
	Unit* target = NULL;

	/* TODO: Different logic for dragoons and zealot */
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

Unit* BattleManager::weakestEnemyInRange(Unit* unit, set<Unit*> enemies) {
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

map< Unit*, set<Unit*> > * BattleManager::getAttackers() {
	map< Unit*, set<Unit*> > * attackedBy = new map< Unit*, set<Unit*> >();

	foreach(Unit* enemy, Broodwar->enemy()->getUnits()) {
		Unit* target = enemy->getOrderTarget();
		if (!target)
			target = enemy->getTarget();

		/* First if: Do we have to react to enemy and is the target of
		 * enemy in our control.
		 */
		if (target && target->getPlayer() == Broodwar->self() && 			
			isInAttackRange(enemy, target) && 
			(this->fighters->find(target) != this->fighters->end()) ) {
			
			/* Have we already made the set of attackers for this unit */
			if ( attackedBy->find(target) != attackedBy->end() )
				(*attackedBy)[target] = set<Unit*>();
			(*attackedBy)[target].insert(enemy);					
		}
	}
	return attackedBy;
}

bool BattleManager::isInAttackRange(Unit* attacker, Unit* target) {
	int attackerRange = attacker->getType().groundWeapon()->maxRange();

	if (attacker->getType() == UnitTypes::getUnitType(string("Protoss Dragoon")))
		// We assume enemy Dragoons have the Singularity Charge upgrade because API doesn't let us check enemy units' upgrade status.
		// We only do the upgrade check for allies.
		if (attacker->getPlayer() == Broodwar->enemy() || attacker->getUpgradeLevel(UpgradeTypes::Singularity_Charge))
			attackerRange = attackerRange * 6 / 4;

	return attacker->getDistance(target) <= attackerRange;
}	

void BattleManager::drawUnitInfo()
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
