/*
 * Histogram.h
 *
 *  Created on: Mar 15, 2013
 *      Author: danke
 */

#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include <iostream>
#include <vector>
#include "Stat.h"

using namespace std;

class Histogram {
protected:
	typedef vector<Stat> VectorType;
	VectorType vec;

public:
	Histogram();
	virtual ~Histogram();

	void clear() {
		vec.clear();
	}

	void addData(int slot, float data);

	void addMaxData(int slot, float data);

	float getSum() {
		float sum = 0;
		for (int i = 0; i < vec.size(); i++)
			sum += vec[i].m_sum;
		return sum;
	}

	float getTimeRange() {
		int maxSlot = 0;
		for (int i = 0; i < vec.size(); i++)
			if (vec[i].m_sum)
				maxSlot = i;
		return maxSlot;
	}

	friend ostream &operator<<(ostream & os, const Histogram &h);
};

ostream &operator<<(ostream & os, const Histogram &h);

#endif /* HISTOGRAM_H_ */
