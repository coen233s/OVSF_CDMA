/*
 * Simulator.h
 *
 *  Created on: Mar 5, 2013
 *      Author: Danke
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <vector>
#include <set>
#include "SimObject.h"

using namespace std;

class Simulator : public SimObject {
protected:
    vector<SimObject *> m_simObjects;
    vector<SimObject *> m_removeObjects;
    int m_time;
public:
    Simulator();
    virtual ~Simulator();

    void addObject(SimObject *obj) {
        m_simObjects.push_back(obj);
    }

    void removeObject(SimObject *obj) {
        m_removeObjects.push_back(obj);
    }

    virtual void onTick(int time);

    // Run until shouldStop() returns true
    virtual void run(bool (*shouldStop)(int time, void *arg), void *arg);

    // Run for timeMax time steps
    virtual void run(int timeMax);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// Random arrival users simular
class RandomArrivalSimulator : public Simulator {
protected:
    std::set<int> freeUserIds;
	std::set<int> usedUserIds;
	int m_maxUserId;
	double m_arrivalRate;

public:
    RandomArrivalSimulator(double arrivalRate=0.1, int maxUserId=250);
    virtual ~RandomArrivalSimulator();

    virtual void onTick(int time);

protected:
	int getNextUserId();
	void freeUserId(int userId);
	int Poisson_Rand(const double lambda);
};


#endif /* SIMULATOR_H_ */
