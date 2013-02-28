/*
 * Transmitter.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include "Transmitter.h"

Transmitter::Transmitter()
: m_pWalshCode(0)
{
}

Transmitter::~Transmitter() {
}

void Transmitter::onTick() {
	if (!m_pWalshCode) {
		m_nextChip = 0;
		return;
	}

	m_nextChip = m_pWalshCode->getChipBit(m_walshIdx);
	if (++m_walshIdx >= m_pWalshCode->length()) {
		m_walshIdx = 0;
	}
}
