/*
 * MobileStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <iostream>
#include <Configuration.h>
#include "MobileStation.h"

using namespace std;

MobileStation::MobileStation(string name, AbsPhyChannel &pch, int uid)
: DeviceBase(name)
, m_phy(pch)
, m_txCtrl(name + ".tx")
, m_protCtrl(m_txCtrl, this)
, m_rxCtrl(name + ".rx", &m_protCtrl)
, m_uid(uid)
, m_minRate(DR_LOW)
, m_maxRate(DR_HIGH)
, m_attached(false)
{
	Configuration &conf(Configuration::getInstance());
	m_txCtrl.setWalshCode(&conf.wcCtrl);

	vector<WHCode *> ctrlCodeSet;
	ctrlCodeSet.push_back(&conf.wcCtrl);
	m_rxCtrl.setWalshCode(ctrlCodeSet);

	m_phy.attachReceiver(&m_rxCtrl);
	m_phy.attachTransmitter(&m_txCtrl);
}

MobileStation::~MobileStation() {
}

void MobileStation::onTick(int time) {
	if (!m_attached) {
		m_attached = true;
		cout << getDeviceId() << ": " << "sending handshake" << endl;
		m_protCtrl.sendHandshake(m_uid, m_minRate, m_maxRate);
	}
}

void MobileStation::onUpdate(void *arg)
{
	ControlFrame &cframe(*(ControlFrame *)arg);
	cout << getDeviceId() << " recv control frame [" << cframe << "]" << endl;
}
