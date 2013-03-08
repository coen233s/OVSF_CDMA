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
#include <phy/AbsPhyChannel.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>
#include <sim/SimObject.h>
#include "protocol/ControlProtocol.h"
#include "DeviceBase.h"

using namespace std;

class Agent : public DeviceBase {
public:
	Transmitter tx;
	Receiver rx;

	Agent(const string& name);
};

class BaseStation : public DeviceBase,
					public UpdateListener,
					public SimObject
{
private:
	AbsPhyChannel &m_phy;			// physical medium
	Transmitter m_txCtrl;			// control channel transmitter
	ControlProtocol m_protCtrl;		// control protocol processor
	Receiver m_rxCtrl;				// control channel receiver
	Assigner m_assigner; // OVSF Code assigner
public:
	BaseStation(const string& name, AbsPhyChannel &pch);
	virtual ~BaseStation();

	// control frame listerner, arg = &ControlFrame
	virtual void onUpdate(void *arg);

	virtual void onTick(int time) {};
};

#endif /* BASESTATION_H_ */
