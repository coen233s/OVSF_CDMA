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
class AbsPhyChannel;

class RandomArrivalSimulator : public Simulator {
public:
	enum TESTMODE {FIXED_LEN, VAR_LEN};  
protected:
    std::set<int> freeUserIds;
	std::set<int> usedUserIds;
	int m_maxUserId;
	double m_arrivalRate;
	AbsPhyChannel& m_physChannel;
	enum TESTMODE m_tmode;

	int m_userDuration;
	float m_packetArrivalRate;
	int m_packetSizeMean;
	int m_packetSizeSD;

public:
    RandomArrivalSimulator(AbsPhyChannel& channel, enum TESTMODE tmode,
            double arrivalRate,
            int userDuration /* ms */,
            float packetArrivalRate,
            int packetSizeMean,
            int packetSizeSD
    );
    virtual ~RandomArrivalSimulator();

    virtual void onTick(int time);

	int getNextUserId();
	void freeUserId(int userId);
	int Poisson_Rand(const double lambda);

	static RandomArrivalSimulator* instance;
};

#endif /* SIMULATOR_H_ */
