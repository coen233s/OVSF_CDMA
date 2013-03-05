/*
 * MobileStation.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef MOBILESTATION_H_
#define MOBILESTATION_H_

#include <phy/AbsPhyChannel.h>
#include <sim/SimObject.h>
#include "DeviceBase.h"
#include "protocol/ControlProtocol.h"

class MobileStation : public DeviceBase,
					  public UpdateListener,
					  public SimObject
{
private:
	AbsPhyChannel &m_phy;			// physical medium
	Transmitter m_txCtrl;			// control channel transmitter
	ControlProtocol m_protCtrl;		// control protocol processor
	Receiver m_rxCtrl;				// control channel receiver

	int m_uid;						// user id
	int m_minRate;
	int m_maxRate;

	bool m_attached;				// ms has attached to bs

public:
	MobileStation(string name, AbsPhyChannel &pch, int uid);
	virtual ~MobileStation();

	virtual void onTick(int time);
	virtual void onUpdate(void *arg);

	void setRateRange(int minRate, int maxRate) {
		m_minRate = minRate;
		m_maxRate = maxRate;
	}
};

#endif /* MOBILESTATION_H_ */
