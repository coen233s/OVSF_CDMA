/*
 * Simulator.h
 *
 *  Created on: Mar 5, 2013
 *      Author: Danke
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <vector>
#include "SimObject.h"

using namespace std;

class Simulator : public SimObject {
protected:
	vector<SimObject *> m_simObjects;
	int m_time;
public:
	Simulator();
	virtual ~Simulator();

	void addObject(SimObject *obj) {
		m_simObjects.push_back(obj);
	}

	virtual void onTick(int time);

	// Run until shouldStop() returns true
	virtual void run(bool (*shouldStop)());

	// Run for timeMax time steps
	virtual void run(int timeMax);
};

#endif /* SIMULATOR_H_ */
