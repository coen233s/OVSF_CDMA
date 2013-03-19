/*
 * Simulator.cpp
 *
 *  Created on: Mar 5, 2013
 *      Author: Danke
 */

#include <algorithm>
#include <assert.h>
#include <Configuration.h>
#include <dev/protocol/ProtocolData.h>
#include <dev/MobileStation.h>
#include "Simulator.h"
#include <math.h>

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

            if ((*it)->isAutoRemove()) {
                SimObject *pSO = (*it);
                delete pSO;
            }
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

#include "dev/MobileStation.h"
#include "debug.h"

class GaussRandom {
public:
	static float randn() {
		return (float) rand() / RAND_MAX;
	}

	static float rand_gauss() {
		float v1,v2,s;

		do {
			v1 = 2.0 * ((float) rand()/RAND_MAX) - 1;
			v2 = 2.0 * ((float) rand()/RAND_MAX) - 1;
			s = v1*v1 + v2*v2;
		} while ( s >= 1.0 );

		if (s == 0.0)
			return 0.0;
		else
			return (v1*sqrt(-2.0 * log(s) / s));
	}

	static float rand_gauss(float mean, float sd) {
		return rand_gauss() * sd + mean;
	}
};

class AutoMobileStation : public MobileStation {
protected:
    typedef MobileStation Base;

    enum STATE {
		STATE_NONE = 0,
        STATE_IDLE,
        STATE_SEND,
        STATE_RECV,
        STATE_DONE,
    } m_state;

    int m_coolingOff;

	int m_time;			// current time
	int m_startTime;	// time the MS started to connect
	int m_totalTime;	// MS life time

	float m_probTransmit;
	float m_pktSizeMean;
	float m_pktSizeSD;

public:
    AutoMobileStation(const string& name, AbsPhyChannel &pch, int uid, bool tr=true,
            int tickDelay = 0,
			void (*cleanUpMobileStation)(MobileStation *) = 0)
    : MobileStation(name, pch, uid, tr, tickDelay, cleanUpMobileStation)
    , m_state(STATE_NONE)
    , m_coolingOff(0)
    { }

	virtual void startTransmit() {
		if (m_state == STATE_NONE) {
			cout << getDeviceId() << m_uid << ": STATE_NONE --> STATE_IDLE" << endl;
			m_state = STATE_IDLE;

			m_startTime = m_time;
                	m_tickCount = m_time;
		}
	}

	void setupParam(int totalTime /* milliseconds */,
					float probTransmit,
					float pktSizeMean,
					float pktSizeSD)
	{
		m_totalTime = totalTime * m_phy.getChipRate() / 1000;
		m_probTransmit = probTransmit;
		m_pktSizeMean = pktSizeMean;
		m_pktSizeSD = pktSizeSD;
	}

	void generateRandomPacket() {
		assert(m_pktSizeMean > 1); // needs to initialize with setupParam()
		
		if (GaussRandom::randn() > m_probTransmit)
			return; // not transmitting at this time step
		
		int packetLen = (int)GaussRandom::rand_gauss(m_pktSizeMean, m_pktSizeSD);

		cout << "Generating packet data: size = " << packetLen << endl;

		for (int i = 0; i < packetLen; i++)
        {
            unsigned char ch = '0' + i % 10;
            
            dout(ch);
            m_pDataChannel->m_tx.pushData(ch);
        }

		dout(endl);
	}

    virtual void onTick(int time) {
        MobileStation::onTick(time);

		m_time = time;

        switch (m_state) {
		case STATE_NONE:
			break;
        case STATE_IDLE:
			if (time > m_startTime + m_totalTime) {
				cout << getDeviceId() << m_uid << ": STATE_IDLE --> STATE_DONE" << endl;
				m_state = STATE_DONE;
				terminate();
				m_tickCount = time - m_tickCount;
			}

			// Initiate data packet
			if (m_tr) {
				generateRandomPacket();
			}

            if (m_pDataChannel && m_pDataChannel->m_tx.hasPendingData()) {
                cout << getDeviceId() << m_uid << ": STATE_IDLE --> STATE_SEND" << endl;
                m_state = STATE_SEND;
            }
            if (m_pDataChannel && m_pDataChannel->m_rx.hasData()) {
                cout << getDeviceId() << m_uid << ": STATE_IDLE --> STATE_RECV" << endl;
                m_state = STATE_RECV;
            }
            break;
        case STATE_SEND:
            if (!m_pDataChannel->m_tx.hasPendingData()) {
                if (m_coolingOff++ > m_pDataChannel->m_tx.getWalshLength()) {
                    cout << getDeviceId() << m_uid << ": STATE_SEND --> STATE_IDLE" << endl;
                    m_state = STATE_IDLE;                                       
                }
            } else {
                m_coolingOff = 0;
            }
            break;
        case STATE_RECV:
            if (!m_pDataChannel->m_rx.hasData()) {
				if (++m_coolingOff > m_pDataChannel->m_rx.getWalshLength()) {
                    cout << getDeviceId() << m_uid << ": STATE_RECV --> STATE_IDLE" << endl;
                    m_state = STATE_IDLE;                                       
                }
            } else {
                m_coolingOff = 0;
            }
            break;
        default:
            break;
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

RandomArrivalSimulator* RandomArrivalSimulator::instance = NULL;

void cleanUpMobileStation2(MobileStation *from)
{
	assert(RandomArrivalSimulator::instance);
	RandomArrivalSimulator::instance->freeUserId(from->getUid());
	RandomArrivalSimulator::instance->removeObject(from);
}

RandomArrivalSimulator::RandomArrivalSimulator(AbsPhyChannel& channel,
	enum TESTMODE tmode,
	double arrivalRate,
	int userDuration /* ms */,
	float packetArrivalRate,
	int packetSizeMean,
	int packetSizeSD
) :
	m_physChannel(channel),
	m_tmode(tmode),
	m_arrivalRate(arrivalRate),
	m_userDuration(userDuration),
	m_packetArrivalRate(packetArrivalRate),
	m_packetSizeMean(packetSizeMean),
	m_packetSizeSD(packetSizeSD)
{
        Configuration &cf(Configuration::getInstance());
        instance = this;

	m_maxUserId = cf.numControlChannelPrio;

	for(int k=2; k < m_maxUserId; k++) {
		freeUserIds.insert(k);
	}
}

RandomArrivalSimulator::~RandomArrivalSimulator()
{
}

void RandomArrivalSimulator::onTick(int time)
{
	int newUserCount = (Poisson_Rand(m_arrivalRate) > 0)? 1: 0;
	if (!freeUserIds.empty()) {
		for (int k=0; k<newUserCount; k++) {
			int userId = getNextUserId();

			if (userId == 0) { // the channel is full
				// assert(false);
				break;
			}

			const int fixedCodeLen = 32;
			int codeLen;
		 	if (m_tmode == FIXED_LEN) {
				codeLen = fixedCodeLen;
			} else {
				int maxLen = 5; //32
				int minLen = 3;

				codeLen = minLen + (rand() % (maxLen - minLen));
				codeLen = 2 << codeLen;
			}

			int rate = m_physChannel.getChipRate() / codeLen;

			cout << "Add mobile station " << userId << " with rate " << rate << endl;

			AutoMobileStation *pMS = new AutoMobileStation("foo", m_physChannel,
			        userId, true, 0, cleanUpMobileStation2);
			pMS->setRateRange(rate, rate);
			pMS->setupParam(m_userDuration, m_packetArrivalRate,
			        m_packetSizeMean, m_packetSizeSD);
			pMS->setAutoRemove(true);
			addObject(pMS);
		}
	}
	Simulator::onTick(time);
}

int RandomArrivalSimulator::getNextUserId()
{
	if (freeUserIds.empty())
		return 0;

	std::set<int>::iterator it = freeUserIds.begin();
	int newUserId = *it;
	assert(newUserId >= 2 && newUserId < m_maxUserId);
	
	freeUserIds.erase(newUserId);
	usedUserIds.insert(newUserId);

	return newUserId;
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
