/*
 * BaseStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <Configuration.h>
#include "BaseStation.h"
#include "protocol/ControlProtocol.h"

Agent::Agent(const string& name)
: DeviceBase(name)
, tx(name + ".tx")
, rx(name + ".rx")
{
}

BaseStation::BaseStation(const string& name, AbsPhyChannel &pch)
: DeviceBase(name)
, m_phy(pch)
, m_txCtrl(name + ".tx")
, m_protCtrl(m_txCtrl, this)
, m_rxCtrl(name + ".rx", &m_protCtrl)
{
	Configuration &conf(Configuration::getInstance());
	m_txCtrl.setWalshCode(&conf.wcCtrl);

	vector<WHCode *> ctrlCodeSet;
	ctrlCodeSet.push_back(&conf.wcCtrl);
	m_rxCtrl.setWalshCode(ctrlCodeSet);

	m_phy.attachReceiver(&m_rxCtrl);
	m_phy.attachTransmitter(&m_txCtrl);
}

BaseStation::~BaseStation() {
}

void BaseStation::onUpdate(void *arg)
{
	ControlFrame &cframe(*(ControlFrame *)arg);
	cout << getDeviceId() << " recv control frame [" << cframe << "]" << endl;
}
