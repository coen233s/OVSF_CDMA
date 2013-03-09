/*
 * BaseStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <Configuration.h>
#include <ovsf.h>
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
	    if (capacity <= 0.0) {
	      // everybody will equally get the same rate.
	      std::vector<std::pair<int,WHCode> > snapshots = m_assigner.listUsedCode();
	      
	      m_assigner.releaseAll();
	      
	      int numberUsers = snapshots.size();
	      int avgCodeLength = 1 << (Math_Log2(numberUsers) + 1);
	      std::vector<int> userIds;
	      std::vector<int> codeLens;
	      for (size_t k=0; k<snapshots.size(); k++) {
		userIds.push_back(snapshots[k].first);
		codeLens.push_back(avgCodeLength);
	      }
	      userIds.push_back(frameOut.uid);
	      codeLens.push_back(avgCodeLength);
	      
	      std::vector<std::pair<int,WHCode> > r = m_assigner.assignUserIds(userIds,codeLens);
	      assert(!r.empty());
	      // We just assign the new code to all users
	      for (size_t k=0; k<r.size(); k++) {
		if (r[k].first == frameOut.uid) {
		  result.first = true;
		  result.second = r[k].second;
		  break;
		}
	      }
	    }
	    else {
	      // we will just give a lowest rate to the new guys
	      int newCodeLength = m_assigner.calcShortestFreeCode(requestCodeLength);
	      cout << "The shortest code length can be assigned without reassigning everything is "
		   << newCodeLength
		   << endl;
	      result = m_assigner.assignUserId(frameOut.uid, newCodeLength);
	      assert(result.first); 
	    }
        }

        // Transmit the code
        std::string byteArray = result.second.toByteArray();
        pCa->length = byteArray.size();

        for (uint16_t i = 0; i < pCa->length; ++i)
        {
            pCa->code[i] = byteArray[i];
        }
#endif
        frameOut.data_size = pCa->length + sizeof(pCa->length);
        char *pframe = reinterpret_cast<char *>(&frameOut);

        const uint16_t size = frameOut.size();

        for (uint16_t i = 0; i < size; i++)
        {
            m_txCtrl.pushData(pframe[i]);
        }
    }
}
