/*
 * Histogram.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: danke
 */

#include <stat/Histogram.h>

Histogram::Histogram() {
}

Histogram::~Histogram() {
}

void Histogram::addData(int slot, float data)
{
	Stat newDat;
	if (vec.size() < slot + 1) {
		vec.resize(slot + 1000, newDat);
	}
	vec[slot].addData(data);
}

ostream &operator<<(ostream & os, const Histogram &h)
{
	for (int i = 0; i < h.vec.size(); i++) {
		os << i << " " << h.vec[i].m_sum << endl;
	}
	return os;
}
