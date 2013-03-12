/*
 * Transmitter.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <iostream>
#include "Transmitter.h"

using namespace std;

Transmitter::Transmitter(const string& name)
: RxTxBase(name)
, m_hasPendingWalsh(false)
, m_walshIdx(0)
, m_BitQueue(name)
, m_pCoupledReceiver(0)
, m_CSMADelay(0)
{
}

Transmitter::~Transmitter() {
}

void Transmitter::onTick(int time) {
    vout(getName() << ": time:" << time << " idx:" << m_walshIdx
            << " walshlen:" << m_walshCode.length()
            << endl);

    m_nextChip = 0;

    if (m_walshCode.length() == 0)
        return;

    // Update Walsh code
    if (m_walshIdx == 0 && m_hasPendingWalsh) {
        m_walshCode = m_pendingWalshCode;
        m_hasPendingWalsh = false;
    }

    // Sync time to multiple of code length
    if (m_walshIdx == 0 && time % m_walshCode.length())
        return;

    // Collision detection (check if channel is busy)
    if (m_pCoupledReceiver) {
        if (m_pCoupledReceiver->getIdleCount() < m_CSMADelay)
            return;
    }

    char walshChip = m_walshCode.getChipBit(m_walshIdx);

    if (m_walshIdx == 0) {
        m_currentBit = m_BitQueue.hasData() ?
                (2 * m_BitQueue.popBit() - 1) : 0;
    }

    m_nextChip = m_currentBit * walshChip;

    if (++m_walshIdx >= m_walshCode.length()) {
        m_walshIdx = 0;
    }
}
