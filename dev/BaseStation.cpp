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

// Calculate the new code length for everybody. This method will give the same code length to everybody.
std::vector<std::pair<int,WHCode> > BaseStation::assignAvgCodeLength(int newUserId)
{
  std::vector<std::pair<int,WHCode> > snapshots = m_assigner.listUsedCode();
	      
  m_assigner.releaseAll();
	      
  int numberUsers = snapshots.size() + 1;
  int avgCodeLength = 1 << (Math_Log2(numberUsers) + 1);

  std::vector<int> userIds;
  std::vector<int> codeLens;
  for (size_t k=0; k<snapshots.size(); k++) {
    userIds.push_back(snapshots[k].first);
    codeLens.push_back(avgCodeLength);
  }
  userIds.push_back(newUserId);
  codeLens.push_back(avgCodeLength);
	      
  return m_assigner.assignUserIds(userIds,codeLens);
}

void BaseStation::transmit(CodeAssignment* pCa, const WHCode& code, ControlFrame& frameOut)
{
  assert(pCa);
 
  std::string byteArray = code.toByteArray();
  pCa->length = byteArray.size();

  for (uint16_t i = 0; i < pCa->length; ++i)
  {
    pCa->code[i] = byteArray[i];
  }

  frameOut.data_size = pCa->length + sizeof(pCa->length);
  char *pframe = reinterpret_cast<char *>(&frameOut);

  const uint16_t size = frameOut.size();
  for (uint16_t i = 0; i < size; i++)
  {
    m_txCtrl.pushData(pframe[i]);
  }
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
#endif 
        if (m_assigner.hasUserId(frameOut.uid)) 
	  return;

        int requestCodeLength = m_assigner.calcShortestFreeCode(4);
        if (!m_assigner.validateRequestCodeLength(requestCodeLength)) 
	  return;
       
        std::pair<bool,WHCode> result = m_assigner.assignUserId(frameOut.uid, requestCodeLength);
        if (!result.first) 
        {
	  // if the capacity is closed too 0.0, it means there is not much space left
	  double capacity = m_assigner.calcCurrentCapacity(); 
	  if (capacity <= 0.0) {
	    std::vector<std::pair<int,WHCode> > r = assignAvgCodeLength(frameOut.uid);
	    assert(!r.empty());

	    // transmit the new code to everybody
	    for (size_t k=0; k<r.size(); k++) {
	      transmit(pCa,r[k].second,frameOut);
	    }
	    return;
	  }
	  // we will just give the lowest rate to the new guys
	  int newCodeLength = m_assigner.calcShortestFreeCode(requestCodeLength);
	  result = m_assigner.assignUserId(frameOut.uid, newCodeLength);
	  assert(result.first); 
	}
	transmit(pCa,result.second,frameOut);
    }
}
