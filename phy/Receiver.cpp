/*
 * Receiver.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <algorithm>
#include <assert.h>
#include <debug.h>
#include <iostream>
#include <vector>
#include <dat/UpdateListener.h>
#include "Receiver.h"

using namespace std;

Receiver::Receiver(const string name, UpdateListener *updateListener)
: RxTxBase(name)
, m_idleCount(0)
, m_maxWalshLen(0)
, m_hasWalshToWait(false)
, m_BitQueue(name, this)
, m_updateListener(updateListener)
{
}

Receiver::~Receiver() {
}

void Receiver::setWalshCode(const vector<WHCode> &newCodes) {
    // copy vector
    m_WalshCode = newCodes;
    m_WalshIdx.clear();
    m_WalshIdx.resize(newCodes.size(), 0);
    m_WalshDotProd.clear();
    m_WalshDotProd.resize(newCodes.size(), 0);

	updateWalshCodeLength();
}

// Change walsh code (in case the walsh code exists, keep its index)
void Receiver::setWalshCode(const WHCode &newCode) {
    int oldIdx, oldSize = m_WalshCode.size();
    int oldCodeIdx = -1, oldCodeDotProd = 0;
    for (oldIdx = 0; oldIdx < oldSize; oldIdx++) {
        if (m_WalshCode[oldIdx] == newCode) {
            oldCodeIdx = m_WalshIdx[oldIdx];
            oldCodeDotProd = m_WalshDotProd[oldIdx];
            break;
        }
    }

    vector<WHCode> vec;
    vec.push_back(newCode);
    setWalshCode(vec);

    if (oldCodeIdx != -1) {
        m_WalshIdx[0] = oldCodeIdx;
        m_WalshDotProd[0] = oldCodeDotProd;
    }
}

void Receiver::updateWalshCodeLength() {
	int maxLen = 0;
	for (vector<WHCode>::iterator it = m_WalshCode.begin();
		it != m_WalshCode.end();
		it++) {
			maxLen = max(maxLen, (*it).length());
	}
	m_maxWalshLen = maxLen;
}

void Receiver::addWalshCode(const WHCode &code) {
    m_WalshCode.push_back(code);
    m_WalshIdx.resize(m_WalshCode.size(), 0);
    m_WalshDotProd.resize(m_WalshCode.size(), 0);

	m_maxWalshLen = max(m_maxWalshLen, code.length());
}

void Receiver::removeWalshCode(const WHCode &code) {
    for (size_t i = 0; i < m_WalshCode.size(); i++) {
        if (m_WalshCode[i] == code) {
            m_WalshCode.erase(m_WalshCode.begin() + i);
            m_WalshIdx.erase(m_WalshIdx.begin() + i);
            m_WalshDotProd.erase(m_WalshDotProd.begin() + i);
            return;
        }
    }
    assert(false); // cannot find the walsh code to delete
}

vector<WHCode> &Receiver::getWalshCode() {
    return m_WalshCode;
}

unsigned char Receiver::popData() {
    unsigned char dat = m_BitQueue.front();
    m_BitQueue.pop_front();
    return dat;
}

int Receiver::peekData(int idx) {
    if (idx < (int)m_BitQueue.size())
        return m_BitQueue.at(idx);
    else
        return -1;
}

void Receiver::onTick(int time) {
    vout(getName() << ": " << time << " idle: " << m_idleCount << endl);

    size_t i;

    for (i = 0; i < m_WalshCode.size(); i++) {
        int codeLen = m_WalshCode[i].length();
        bool walshHit = false;

        // Sync time to multiple of code length
        if (m_WalshIdx[i] == 0 && time % codeLen)
            continue;

        m_WalshDotProd[i] += m_LastChip *
                m_WalshCode[i].getChipBit(m_WalshIdx[i]);

        vout(getName() << " lastChip: " << m_LastChip <<
                " walsh idx: " << (int)m_WalshIdx[i] <<
                " walsh len: " << (int)codeLen <<
                " dot prod: " << (int)m_WalshDotProd[i] << endl);

        // Received one bit
        if (++m_WalshIdx[i] >= codeLen) {
            if (m_WalshDotProd[i] == codeLen) {
                m_BitQueue.pushBit(1); // received 1
                m_idleCount = 0;
                walshHit = true;
            } else if (m_WalshDotProd[i] == -codeLen) {
                m_BitQueue.pushBit(0); // received 0
                m_idleCount = 0;
                walshHit = true;
            } else if (m_WalshDotProd[i] != 0) {
                /* this is not a real issue - just means someone else is talking
                 * on the channel */
                /*
				cout << "Warning: " << "Receiver [" << getName() <<
						"] got unexpected dot product " << m_WalshDotProd[i] <<
						" codeLen: " << codeLen << endl;
                 */
            } else {
                // Channel is quiet
                m_idleCount++;
            }

            // clear idx & dot product
            m_WalshIdx[i] = 0;
            m_WalshDotProd[i] = 0;

            // Check walsh to wait (3-way code change protocol)
            if (m_hasWalshToWait && walshHit && m_WalshCodeToWait == m_WalshCode[i]) {
                cout << getName() << ": got bit encoded with new Walsh, drop old code (if any)"
                        << endl;
                setWalshCode(m_WalshCodeToWait);
                m_hasWalshToWait = false;
                break;
            }
        }
    }
}

void Receiver::onUpdate(void *arg)
{
    if (m_updateListener)
        m_updateListener->onUpdate(this);
}
