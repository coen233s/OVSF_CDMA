/*
 * BaseStation.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef BASESTATION_H_
#define BASESTATION_H_

#include <vector>
#include <ovsf.h>
#include <unordered_map>
#include <dev/DataChannel.h>
#include <phy/AbsPhyChannel.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>
#include <sim/SimObject.h>
#include "protocol/ControlProtocol.h"
#include "DeviceBase.h"

using namespace std;

class BaseStation : public DeviceBase,
                    public UpdateListener,
                    public SimObject
{
public:
    enum MODE {
        FIXED_ONCE = 1,
		FIXED_DYNAMIC,
		VAR_ONCE,
		VAR_DYNAMIC
    };

protected:
    typedef map<int, DataChannel *> DataChannelMap;
    typedef map<int, WHCode> UserWalshMap;

    AbsPhyChannel &m_phy;			// physical medium
    Transmitter m_txCtrl;			// control channel transmitter
    ControlProtocol m_protCtrl;		// control protocol processor
    Receiver m_rxCtrl;				// control channel receiver
	AssignerInterface* m_pAssigner; // OVSF Code assigner
    DataChannelMap m_dataChannel;	// Data channels to talk to MobileStations
    UserWalshMap m_pendingWalsh;    // Map UID to Walsh Code
    MODE m_mode; // Code Assignment mode

    int m_totalConnectNum;          // total connection number (for automation)
                                    // incremented every time some ms is connected

    int m_totalDisconnectNum;       // total disconnections (for automation)

    static const int CTRL_USERID;     // ID for the control channel - to distinguish from other users
    static const int CTRL_CODELEN; // # of bits of control channel's Walsh code

public:
    BaseStation(const string& name, AbsPhyChannel &pch, MODE mode = VAR_DYNAMIC);
    virtual ~BaseStation();

    // control frame listerner, arg = &ControlFrame
    virtual void onUpdate(void *arg);

    // on data in
    virtual void onDataIn(int id, char ch) {}

    virtual void onTick(int time) {}

    int getTotalConnections() {
        return m_totalConnectNum;
    }

    int getTotalDisconnections() {
        return m_totalDisconnectNum;
    }

protected:
    std::vector<std::pair<int,WHCode> > assignAvgCodeLength(int newUserId);
    void transmit(CodeAssignment* pCa, const WHCode& code, ControlFrame& frameOut);
    // uid - user, tr - transmit/receiver (data channel type), minRate/maxRate in bps
    void addUser(int uid, int tr, int minRate, int maxRate);
    void removeUser(int uid, int tr);
    // Add code to a channel (this method can be invoked to update the code too)
    virtual void addChannel(int uid, int tr, const WHCode &code);
    void removeChannel(int uid, int tr);
    int rateToCodeLength(int dataRate);

    WHCode initControlChannelWalshCode(const int id, const int codelen);
};

#endif /* BASESTATION_H_ */
