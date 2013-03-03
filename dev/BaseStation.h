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
#include "protocol/ControlProtocol.h"
#include "DeviceBase.h"

using namespace std;

class Agent : public DeviceBase {
public:
	Transmitter tx;
	Receiver rx;

	Agent(string name);
};

class BaseStation : public DeviceBase {
private:
	AbsPhyChannel &m_phy;			// physical medium
	ControlProtocol m_protCtrl;		// control protocol processor
	Receiver m_rxCtrl;				// control channel receiver
	Transmitter m_txCtrl;			// control channel transmitter

public:
	BaseStation(string &name, AbsPhyChannel &pch);
	virtual ~BaseStation();
};

#endif /* BASESTATION_H_ */
