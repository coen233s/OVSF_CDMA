/*
 * BitOutQueue.cpp
 *
 *  Implement a queue outputting bits. Bit order is little endian.
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <stdexcept>
#include "BitOutQueue.h"

BitOutQueue::BitOutQueue()
: m_hasDataInByteBuffer(false)
, m_bitMask(1)
, m_dataByte(0)
{
}

BitOutQueue::~BitOutQueue() {
}

bool BitOutQueue::hasData()
{
	return m_hasDataInByteBuffer || !empty();
}

char BitOutQueue::popBit()
{
	if (!m_hasDataInByteBuffer) {
		if (empty()) {
			throw logic_error("BitOutQueue::popBit: queue empty");
		} else {
		    m_hasDataInByteBuffer = true;
			m_dataByte = front();
			pop();
			m_bitMask = 1;
		}
	}

	char bitOutput = !!(m_dataByte & m_bitMask);

	// byte boundary
	if (m_bitMask & 0x80)
		m_hasDataInByteBuffer = false;
	else
		m_bitMask <<= 1;

	return bitOutput;
}
