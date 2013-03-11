/*
 * MobileStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <iostream>
#include <sstream>
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
, m_tr(tr)
, m_tickDelay(tickDelay)
, m_attached(false)
, m_pDataChannel(0)
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
    if (0 != m_pDataChannel)
    {
        delete m_pDataChannel;
        m_pDataChannel = 0;
    }
}

void MobileStation::onTick(int time) {
	if ((!m_attached) && (time >= m_tickDelay)){
		m_attached = true;
		cout << getDeviceId() << ": " << "sending handshake" << endl;
		m_protCtrl.sendHandshake(m_uid, m_minRate, m_maxRate, m_tr);
	}

    if (0 != m_pDataChannel) 
    {
        if (m_tr) // Transmit
        {
            static char msg[] = "hello";
            static int i = 0;
            if (i < sizeof(msg) - 1)
            {
                cout << getDeviceId() << ": " << "sending data" << endl;
                m_pDataChannel->m_tx.pushData(msg[i++]);
            }
        }
        else // Receive
        {
        }
    }
}

void MobileStation::onUpdate(void *arg)
{
    ControlFrame &cframe(*(ControlFrame *)arg);
    if (!cframe.c2s && cframe.uid == m_uid)
    {
        cout << getDeviceId() << " recv control frame \n[" << cframe << "]" << endl;
        ostringstream convertId;
        convertId << getDeviceId() << "." << m_uid;
        string chanstr = convertId.str();
        if (0 == m_pDataChannel)
        {
            m_pDataChannel = new DataChannel(chanstr, m_phy);
        }
        

	CodeAssignment *pCa = reinterpret_cast<CodeAssignment *>(&cframe.data);
	std::string byteArray = "";
	for (uint32_t i = 0; i < pCa->length; ++i)
        {
	  byteArray += ((char)pCa->code[i]);
        }
	WHCode code(byteArray);

        if (m_tr)
        {
	  m_pDataChannel->setRxWalshCode(code);
            // TODO: start sending data
        }
        else
        {
	  m_pDataChannel->setTxWalshCode(code);
            // TODO: start receiving data
        }
    }
}
