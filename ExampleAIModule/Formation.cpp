#include "Formation.h"

using namespace helpers;

Formation::Formation()
{
	this->distances = this->p1 = this->angle = 0;
	this->form = free_will;
	this->units = 0;
}

Formation::Formation(Formations f, std::set<BWAPI::Unit*>* units)
{
	this->units = units;
	this->form = f;
	switch(f) {
		case parabola:
			this->p1 = 0.2;
			this->distances = BWAPI::TILE_SIZE;
			this->angle = 0;
		default:
			this->distances = this->p1 = this->angle = 0;
	}
}

void Formation::moveInFormation(BWAPI::Position to)
{
	double newangle = angleBetween(to, cos(this->angle), sin(this->angle));
	if (fabs(angle - newangle) > 0.001) {
		this->angle = newangle;
		// rotate the formation
	}
	else {
		// keep the course
	}
}