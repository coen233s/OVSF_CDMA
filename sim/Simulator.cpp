/*
 * Simulator.cpp
 *
 *  Created on: Mar 5, 2013
 *      Author: Danke
 */

#include <algorithm>
#include <assert.h>
#include "Simulator.h"

Simulator::Simulator() {
}

Simulator::~Simulator() {
}

void Simulator::onTick(int time) {
    // Remove deleted objects
    if (m_removeObjects.size()) {
        for (vector<SimObject *>::iterator it = m_removeObjects.begin();
                it != m_removeObjects.end();
                it++) {
            m_simObjects.erase(find(m_simObjects.begin(), m_simObjects.end(),
                    (*it)));
        }
        m_removeObjects.clear();
    }

    for (vector<SimObject *>::iterator it = m_simObjects.begin();
            it != m_simObjects.end();
            it++)
    {
        (*(it))->onTick(time);
    }
}

// Run until shouldStop() returns true
void Simulator::run(bool (*shouldStop)(int time, void *arg), void *arg) {
    m_time = 0;
    while (!shouldStop(m_time, arg)) {
        onTick(m_time);
        m_time++;
    }
}

// Run for timeMax time steps
void Simulator::run(int timeLim) {
    for (m_time = 0; m_time < timeLim; m_time++) {
        onTick(m_time);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RandomArrivalSimulator::RandomArrivalSimulator(double arrivalRate, int maxUserId):
	m_arrivalRate(arrivalRate),
	m_maxUserId(maxUserId)
{
	for(int k=2; k<=maxUserId; k++) {
		freeUserIds.insert(k);
	}
}

RandomArrivalSimulator::~RandomArrivalSimulator()
{
}

void RandomArrivalSimulator::onTick(int time)
{
	int newUserCount = Poisson_Rand(m_arrivalRate);
	for (int k=0; k<newUserCount; k++) {
		int userId = getNextUserId();
		if (userId == 0) { // the channel is full
			assert(false);
			break;
		}

		//addObject(user(id));
	}
	Simulator::onTick(time);
}

int RandomArrivalSimulator::getNextUserId()
{
	if (freeUserIds.empty())
		return 0;

	std::set<int>::iterator it = freeUserIds.begin();
	int newUserId = *it;
	assert(newUserId >= 2 && newUserId <= m_maxUserId);
	
	freeUserIds.erase(newUserId);
	usedUserIds.insert(newUserId);
}

void RandomArrivalSimulator::freeUserId(int userId)
{
	usedUserIds.erase(userId);
	freeUserIds.insert(userId);
}

int RandomArrivalSimulator::Poisson_Rand(const double lambda)
{
	double r = -1.0 * (double)lambda;
	double L = exp(r);
	int k = 0;
	double p = 1.0;
	do {
		k = k + 1;
		double u =((double)rand()/(double)RAND_MAX);
		p = p * u;
	} while (p > L);
	return k - 1;
}