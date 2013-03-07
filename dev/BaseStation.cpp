/*
 * BaseStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <Configuration.h>
#include <string.h>
#include "BaseStation.h"
#include "protocol/ControlProtocol.h"

Agent::Agent(string name)
: DeviceBase(name)
, tx(name + ".tx")
, rx(name + ".rx")
{
}

BaseStation::BaseStation(string name, AbsPhyChannel &pch)
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

    if (cframe.c2s)
    {
        cout << getDeviceId() << " recv control frame \n[" << cframe << "]" << endl;

#if 1 // Faking the code assignment
        ControlFrame frameOut;
        frameOut.uid = cframe.uid;
        frameOut.c2s = 0;
        frameOut.tr = cframe.tr;
        CodeAssignment *pCa = reinterpret_cast<CodeAssignment *>(&frameOut.data);

        enum { ARBITRARY_LEN = 4 };
        memset(pCa->code, 0xAB, ARBITRARY_LEN);
        pCa->length = ARBITRARY_LEN;
        pCa->code[ARBITRARY_LEN - 1] = 0xCD;
        frameOut.data_size = ARBITRARY_LEN + 1;
        char *pframe = reinterpret_cast<char *>(&frameOut);

        const uint16_t size = frameOut.size();
        cout << getDeviceId() << " send code assignment\n";

        for (uint16_t i = 0; i < size; i++)
        {
            m_txCtrl.pushData(pframe[i]);
        }
#endif
    }
}
