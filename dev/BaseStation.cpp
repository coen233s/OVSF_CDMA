/*
 * BaseStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <Configuration.h>
#include <string.h>
#include <assert.h>
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

    if (cframe.c2s)
    {
        cout << getDeviceId() << " recv control frame \n[" << cframe << "]" << endl;

        ControlFrame frameOut;
        frameOut.uid = cframe.uid;
        frameOut.c2s = 0;
        frameOut.tr = cframe.tr;
        CodeAssignment *pCa = reinterpret_cast<CodeAssignment *>(&frameOut.data);
        cout << getDeviceId() << " send code assignment\n";

#if 0 // Faking the code assignment
        enum { ARBITRARY_LEN = 4 };
        memset(pCa->code, 0xAB, ARBITRARY_LEN);
        pCa->length = ARBITRARY_LEN;
        pCa->code[ARBITRARY_LEN - 1] = 0xCD;
        frameOut.data_size = ARBITRARY_LEN + 1;
        char *pframe = reinterpret_cast<char *>(&frameOut);

        const uint16_t size = frameOut.size();

        for (uint16_t i = 0; i < size; i++)
        {
            m_txCtrl.pushData(pframe[i]);
        }
#else 
        if (m_assigner.hasUserId(frameOut.uid)) 
        {
	  cout << "UserId " << frameOut.uid << " already has WHCode." << endl;
	  cout << "Do you want to give him a new code and discard the old one?" << endl;
	  return;
	}

	int requestCodeLength = 8; // the shorter code, the higher bit rate.
	if (!m_assigner.validateRequestCodeLength(requestCodeLength)) 
	{
	  cout << "Error: Request the code length that > 2 and a power of 2." << endl;
	  return;
	}

        std::pair<bool,WHCode> result = m_assigner.assignUserId(frameOut.uid, requestCodeLength);
	if (!result.first) 
	{
	  // If this happens, it means there is not enough 8-bits code. 
	  
	  // Capacity - if it is closed too 0.0, it means there is not much space left
	  double capacity = m_assigner.calcCurrentCapacity(); 
       	  cout << "Current Capacity is " << capacity << endl; 

	  int newCodeLength = m_assigner.calcShortestFreeCode(requestCodeLength);
	  cout << "The shortest code length can be assigned without reassigning everything is "
	       << newCodeLength
	       << endl;
	  
	  // investigate the code tree.
	  m_assigner.print();

	  // Fix suggestion # 1: - just give that possible shortest code
	  result = m_assigner.assignUserId(frameOut.uid, newCodeLength);
	  assert(result.first); 
	  // It should not assert because this code length is always available.
	  // If it asserts, there might be a potential race condition.

	  // Fix suggestion # 2: Reassigning new code for everybody in the channel. 
	  // This is a search problem. You need to slowly increase some users bandwidth until
	  // the assignment is accepted. -- We will discuss about this one offline.

	  // Fix suggestion # 3: Just block this new user.
	}

	// Transmit the code
	std::string byteArray = result.second.toByteArray();
	pCa->length = byteArray.size();
	m_txCtrl.pushData(pCa->length);

	for (uint16_t i = 0; i < pCa->length; ++i)
	{
	  m_txCtrl.pushData(byteArray[i]);
	}
#endif
    }
}
