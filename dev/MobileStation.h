/*
 * MobileStation.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef MOBILESTATION_H_
#define MOBILESTATION_H_

#include <sstream>
#include <phy/AbsPhyChannel.h>
#include <sim/SimObject.h>
#include <dev/DataChannel.h>
#include "DeviceBase.h"
#include "protocol/ControlProtocol.h"

using namespace std;

class MobileStation : public DeviceBase,
                      public UpdateListener,
                      public SimObject
{
protected:
    AbsPhyChannel &m_phy;			// physical medium
    Transmitter m_txCtrl;			// control channel transmitter
    ControlProtocol m_protCtrl;		// control protocol processor
    Receiver m_rxCtrl;				// control channel receiver

    int m_uid;						// user id
    int m_minRate;
    int m_maxRate;
    bool m_tr;
    int m_tickDelay;
    int m_tickCount;

    bool m_attached;				// ms has attached to bs
    bool m_terminated;
    DataChannel* m_pDataChannel;
    bool validateRate(int rate);

	void (*m_cleanUpMobileStation)(int);

private:
    static string getCtrlIdString(const string& name, int uid) {
        ostringstream  os;
        os << name << ".ctl" << uid;
        return os.str();
    }

protected:
	virtual void startTransmit() {};

	// called when mobile station object can be removed
	virtual void onCleanup() {
		cout << getDeviceId() << ": calling onCleanup() callback" << endl;
		if (m_cleanUpMobileStation)
			m_cleanUpMobileStation(m_uid);
	};

public:
    MobileStation(const string& name, AbsPhyChannel &pch, int uid, bool tr=true, int tickDelay = 0,
		void (*cleanUpMobileStation)(int) = 0);
    virtual ~MobileStation();

    virtual void onTick(int time);
    virtual void onUpdate(void *arg);

    void terminate();

    void setRateRange(int minRate, int maxRate) {
        validateRate(minRate);
        validateRate(maxRate);
        m_minRate = minRate;
        m_maxRate = maxRate;
    }
};

#endif /* MOBILESTATION_H_ */
