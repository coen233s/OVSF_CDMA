/*
 * BaseStation.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <Configuration.h>
#include <ovsf.h>
#include <sstream>
#include <string.h>
#include <assert.h>
#include "BaseStation.h"
#include "protocol/ControlProtocol.h"

using namespace std;

const int BaseStation::CTRL_USERID = 1;
const int BaseStation::CTRL_CODELEN = 16; // 16-bit fixed-Walshcode

BaseStation::BaseStation(const string& name, AbsPhyChannel &pch, MODE mode)
: DeviceBase(name)
, m_phy(pch)
, m_txCtrl(name + ".tx")
, m_protCtrl(m_txCtrl, this)
, m_rxCtrl(name + ".rx", &m_protCtrl)
, m_mode(mode)
{
    WHCode ctrlCode = initControlChannelWalshCode(CTRL_USERID,CTRL_CODELEN);
    Configuration &conf(Configuration::getInstance());
    conf.setControlChannelCode(ctrlCode);

    m_txCtrl.setWalshCode(conf.wcCtrl);
    m_rxCtrl.setWalshCode(conf.wcCtrl);

    m_phy.attachReceiver(&m_rxCtrl);
    m_phy.attachTransmitter(&m_txCtrl);

    m_txCtrl.setCSMA(&m_rxCtrl, 1); // priority 1
}

BaseStation::~BaseStation() {
}

WHCode BaseStation::initControlChannelWalshCode(const int id, const int codelen)
{
    // These constraits must be satisfied before we are running the basestation
    assert(m_assigner.validateRequestCodeLength(codelen));
    assert(id > 0);
    assert(m_assigner.calcCurrentCapacity() == 1.0);
    assert(m_assigner.calcShortestFreeCode(codelen) <= codelen);

    std::pair<bool,WHCode> rval = m_assigner.assignUserId(id,codelen);
    assert(rval.first);
    return rval.second;
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

    // Update UID's walsh code
    if (frameOut.tr) {
        // MobileStation is Tx - add new code but do not remove old code until
        // it has ack'ed
        m_pendingWalsh[frameOut.uid] = code;
        addChannel(frameOut.uid, frameOut.tr, code);
    } else {
        // MobileStation is Rx - do not change the code until it is ack'ed
        // do nothing here
    }

    std::string byteArray = code.toByteArray();
    pCa->length = byteArray.size();

    for (uint16_t i = 0; i < pCa->length; ++i)
    {
        pCa->code[i] = byteArray[i];
    }

    frameOut.data_size = pCa->length + sizeof(pCa->length);
    char *pframe = reinterpret_cast<char *>(&frameOut);

    const uint16_t size = frameOut.size();
    dout(getName() << ": transmit len=" << (int)size << hex);

    for (uint16_t i = 0; i < size; i++)
    {
        m_txCtrl.pushData(pframe[i]);
        dout(" " << (unsigned)(0xFF & pframe[i]));
    }

    dout(dec << endl);
}

int BaseStation::rateToCodeLength(int dataRate)
{
    int len = m_phy.getChipRate() / dataRate;
    if (len < 4)
        len = 4;
    return len;
}

void BaseStation::addChannel(int uid, int tr, const WHCode &code)
{
    DataChannelMap::iterator dc = m_dataChannel.find(uid);
    DataChannel *pdc;

    // Add a new DataChannel object or use the old one, if the same uid
    // is requesting a second data channel (each can request up to 2, one
    // tx and one rx)
    if (dc == m_dataChannel.end()) {
        ostringstream convertId;
        convertId << getDeviceId() << "." << uid;
        string chanstr = convertId.str();
        pdc = new DataChannel(chanstr, m_phy);
        m_dataChannel[uid] = pdc;
    } else {
        pdc = dc->second;
    }

    // If tr == 1, BaseStation receives data from MobileStation,
    // If tr == 0, BaseStatoin sends data to MobileStation
    if (tr)
        pdc->addRxWalshCode(code);
    else
        pdc->setTxWalshCode(code);
}

void BaseStation::removeChannel(int uid, int tr)
{
    DataChannelMap::iterator dc = m_dataChannel.find(uid);
    DataChannel *pdc;

    // find the channel
    if (dc == m_dataChannel.end()) {
        // should not remove a channel that does not exist
        assert(false);
    } else {
        pdc = dc->second;
        if (tr)
            pdc->removeRx();
        else
            pdc->removeTx();
        if (pdc->canFreeChannel()) {
            m_dataChannel.erase(dc);
            delete pdc;
        }
    }

    UserWalshMap::iterator walshIter = m_pendingWalsh.find(uid);
    if (walshIter != m_pendingWalsh.end())
        m_pendingWalsh.erase(walshIter);
}

void BaseStation::addUser(int uid, int tr, int minRate, int maxRate)
{
    assert(minRate <= maxRate);

    int maxCodeLen = rateToCodeLength(minRate);
    int minCodeLen = rateToCodeLength(maxRate);

    ControlFrame frameOut;
    frameOut.uid = uid;
    frameOut.c2s = 0;
    frameOut.tr = tr;
    CodeAssignment *pCa = reinterpret_cast<CodeAssignment *>(&frameOut.data);

    cout << getDeviceId() << " send code assignment\n";

    if (m_assigner.hasUserId(frameOut.uid)) {
        cerr << "Error: " << getDeviceId() << ": user has already connected, " <<
                "uid: " << (int)frameOut.uid << endl;
        return;
    }

    int requestCodeLength = maxCodeLen;
    if (!m_assigner.validateRequestCodeLength(requestCodeLength)) {
        cerr << "Error: " << getDeviceId() << ": valiate request code length failed," <<
                " maxCodeLen: " << maxCodeLen <<
                " requestCodeLength: " << requestCodeLength << endl;
        return;
    }

    // Trying to specify code length range
#if !TEST_CODE_RANGE
    std::pair<bool,WHCode> result = m_assigner.assignUserId(frameOut.uid, requestCodeLength);
#else
    std::pair<bool,WHCode> result = m_assigner.assignUserId(frameOut.uid,
            minCodeLen, requestCodeLength);
#endif

    if (!result.first)
    {
        // if the capacity is closed too 0.0, it means there is not much space left
        double capacity = m_assigner.calcCurrentCapacity();
        if (capacity <= 0.0) {
            std::vector<std::pair<int,WHCode> > r = assignAvgCodeLength(frameOut.uid);
            assert(!r.empty());

            // transmit the new code to everybody
            for (size_t k=0; k<r.size(); k++) {
	        if(r[k].first != 0) {
		    //int userId = r[k].first;
		    //WHCode& wcode = r[k].second;
		    transmit(pCa,r[k].second,frameOut);
		}
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

void BaseStation::removeUser(int uid, int tr)
{
    // XXX m_assigner does not allow 2 codes for each user. But this is
    // supported by BaseStation (each uid can have two subchannels, rx and tx),
    // To get full duplex support, we can add 'tr' parameter to Assigner.
    m_assigner.releaseUserId(uid);
    removeChannel(uid, tr);
}

void BaseStation::onUpdate(void *arg)
{
    ControlFrame &cframe(*(ControlFrame *)arg);

    if (cframe.c2s)
    {
        cout << getDeviceId() << " recv control frame \n[" << cframe << "]" << endl;

        if (!cframe.ack) {
            if (cframe.req) {
                // process the channel request
                RateRequest *pReq = reinterpret_cast<RateRequest *>(&cframe.data);
                // send walsh code to the calling user, or to all users if code
                // needs reassignment
                addUser(cframe.uid, cframe.tr, pReq->min_rate, pReq->max_rate);
            } else {
                // shutdown the channel
                removeUser(cframe.uid, cframe.tr);
            }
        } else {
            cout << getDeviceId() << " recv ack from uid " << cframe.uid << " tr: "
                    << (int)cframe.tr << endl;

            if (cframe.tr) {
                cout << getDeviceId() << " use only new code to recv uid " << cframe.uid
                     << endl;
                // Client ack'ed the code, now the old code can be removed
                m_dataChannel[cframe.uid]->setRxWalshCode(m_pendingWalsh[cframe.uid]);
            } else {
                cout << getDeviceId() << " start new code to send to uid " << cframe.uid
                     << " (drop old code if any)" << endl;
                addChannel(cframe.uid, cframe.tr, m_pendingWalsh[cframe.uid]);
            }
        }
    }
}
