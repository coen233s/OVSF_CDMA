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

MobileStation::MobileStation(const string& name, AbsPhyChannel &pch, int uid, bool tr, int tickDelay)
: DeviceBase(name)
, m_phy(pch)
, m_txCtrl(name + ".tx")
, m_protCtrl(m_txCtrl, this)
, m_rxCtrl(name + ".rx", &m_protCtrl)
, m_uid(uid)
, m_minRate(RATE_MIN)
, m_maxRate(RATE_MAX)
, m_attached(false)
, m_tr(tr)
, m_tickDelay(tickDelay)
{
	Configuration &conf(Configuration::getInstance());
	m_txCtrl.setWalshCode(conf.wcCtrl);

	vector<WHCode> ctrlCodeSet;
	ctrlCodeSet.push_back(conf.wcCtrl);
	m_rxCtrl.setWalshCode(ctrlCodeSet);

	m_phy.attachReceiver(&m_rxCtrl);
	m_phy.attachTransmitter(&m_txCtrl);

	// priority = uid
	m_txCtrl.setCSMA(&m_rxCtrl, m_uid);
}

MobileStation::~MobileStation() {
}

void MobileStation::onTick(int time) {
	if ((!m_attached) && (time >= m_tickDelay)){
		m_attached = true;
		cout << getDeviceId() << ": " << "sending handshake" << endl;
		m_protCtrl.sendHandshake(m_uid, m_minRate, m_maxRate, m_tr);
	}
}

void MobileStation::onUpdate(void *arg)
{
	ControlFrame &cframe(*(ControlFrame *)arg);
    if (!cframe.c2s && cframe.uid == m_uid)
    {
    	cout << getDeviceId() << " recv control frame \n[" << cframe << "]" << endl;
    	// TODO create a Data Channel when receiving Walsh Code
    }
}
