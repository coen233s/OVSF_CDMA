/*
 * Transmitter.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <iostream>
#include "Transmitter.h"

using namespace std;

Transmitter::Transmitter(const string& name)
: RxTxBase(name)
, m_pWalshCode(0)
{
}

Transmitter::~Transmitter() {
}

void Transmitter::onTick() {
	if (!m_pWalshCode) {
		m_nextChip = 0;
		return;
	}

	char walshChip = m_pWalshCode->getChipBit(m_walshIdx);

	if (m_walshIdx == 0) {
	    m_currentBit = m_BitQueue.hasData() ?
	            (2 * m_BitQueue.popBit() - 1) : 0;
	}

	m_nextChip = m_currentBit * walshChip;

	if (++m_walshIdx >= m_pWalshCode->length()) {
		m_walshIdx = 0;
	}
}
