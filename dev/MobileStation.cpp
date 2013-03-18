/*
 * MobileStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <Configuration.h>
#include "MobileStation.h"
#include <stdio.h>

using namespace std;

MobileStation::MobileStation(const string& name, AbsPhyChannel &pch, int uid, bool tr, int tickDelay)
: DeviceBase(name)
, m_phy(pch)
, m_txCtrl(getCtrlIdString(name, uid) + ".tx")
, m_protCtrl(m_txCtrl, this)
, m_rxCtrl(getCtrlIdString(name, uid) + ".rx", &m_protCtrl)
, m_uid(uid)
, m_minRate(RATE_MIN)
, m_maxRate(RATE_MAX)
, m_tr(tr)
, m_tickDelay(tickDelay)
, m_tickCount(0)
, m_attached(false)
, m_terminated(false)
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
        cout << getDeviceId() << m_uid << ": " << "sending handshake" << endl;
        m_protCtrl.sendHandshake(m_uid, m_minRate, m_maxRate, m_tr);
    }
}

void MobileStation::onUpdate(void *arg)
{
    ControlFrame &cframe(*(ControlFrame *)arg);
    if (!cframe.c2s && cframe.uid == m_uid)
    {
        cout << getDeviceId() << m_uid << " recv control frame \n[" << cframe << "]" << endl;
        ostringstream convertId;
        convertId << getDeviceId() << "." << m_uid;
        string chanstr = convertId.str();
        if (0 == m_pDataChannel)
        {
            cout << getDeviceId() << m_uid << ": bringing up data channel" << endl;
            m_pDataChannel = new DataChannel(chanstr, m_phy, !m_tr);
        }

        CodeAssignment *pCa = reinterpret_cast<CodeAssignment *>(&cframe.data);
        std::string byteArray = "";
        for (uint32_t i = 0; i < pCa->length; ++i)
        {
            byteArray += ((char)pCa->code[i]);
        }
        WHCode code(byteArray);
        cout << getDeviceId() << m_uid << ": got walshcode (len " << code.length() << "): "
	     << code
	     << endl;

        if (m_tr)
        {
            m_pDataChannel->setTxWalshCode(code);

            // Acknowledge the new code
            cout << getDeviceId() << m_uid << ": ack the walsh code for tx channel" << endl;
            m_protCtrl.sendCodeAck(m_uid, m_tr);

            // TODO: start sending data
            startTransmit();
        }
        else
        {
            m_pDataChannel->addRxWalshCode(code);

            // Acknowledge the new code
            cout << getDeviceId() << m_uid << ": ack and add the walsh code for rx channel" << endl;
            m_protCtrl.sendCodeAck(m_uid, m_tr);

            // TODO: start receiving data
        }
    }
}

void MobileStation::terminate()
{
    cout << getDeviceId() << m_uid << ": Terminating..." << endl;
    m_protCtrl.sendTearDown(m_uid, m_tr);
    delete m_pDataChannel;
    m_pDataChannel = 0;
    ostringstream convertId;
    convertId << getDeviceId() << "." << m_uid;
    string name = convertId.str() + ".tick.txt";
    FILE* f = fopen(name.c_str(), "w");
    fprintf(f, "%d", m_tickCount);
    fclose(f);
}

bool MobileStation::validateRate(int rate)
{
    switch(rate)
    {
    case RATE_1200:
    case RATE_2400:
    case RATE_4800:
    case RATE_9600:
    case RATE_19200:
    case RATE_38400:
    case RATE_76800:
    case RATE_153600:
    case RATE_307K:
    case RATE_614K:
    case RATE_1M2:
    case RATE_2M4:
    case RATE_4M9:
    case RATE_9M8:
    case RATE_19M:
    case RATE_39M:
        return true;
    default:
        assert(0);
        return false;
    }
}
