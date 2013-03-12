/*
 * Receiver.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <iostream>
#include <dat/UpdateListener.h>
#include "Receiver.h"

using namespace std;

Receiver::Receiver(const string name, UpdateListener *updateListener)
: RxTxBase(name)
, m_idleCount(0)
, m_BitQueue(name, this)
, m_updateListener(updateListener)
{
}

Receiver::~Receiver() {
}

void Receiver::setWalshCode(vector<WHCode> newCodes) {
    // copy vector
    m_WalshCode = newCodes;
    m_WalshIdx.clear();
    m_WalshIdx.resize(newCodes.size(), 0);
    m_WalshDotProd.clear();
    m_WalshDotProd.resize(newCodes.size(), 0);
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

        // Sync time to multiple of code length
        if (m_WalshIdx[i] == 0 && time % codeLen)
            continue;

        m_WalshDotProd[i] += m_LastChip *
                m_WalshCode[i].getChipBit(m_WalshIdx[i]);

        vout(getName() << " lastChip: " << m_LastChip <<
                " walsh idx: " << (int)m_WalshCode[i].getChipBit(m_WalshIdx[i]) <<
                " dot prod: " << (int)m_WalshDotProd[i] << endl);

        // Received one bit
        if (++m_WalshIdx[i] >= codeLen) {
            if (m_WalshDotProd[i] == codeLen) {
                m_BitQueue.pushBit(1); // received 1
                m_idleCount = 0;
            } else if (m_WalshDotProd[i] == -codeLen) {
                m_BitQueue.pushBit(0); // received 0
                m_idleCount = 0;
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
        }
    }
}

void Receiver::onUpdate(void *arg)
{
    if (m_updateListener)
        m_updateListener->onUpdate(this);
}
