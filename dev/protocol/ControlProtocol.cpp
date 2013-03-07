/*
 * ControlProtocol.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <iostream>
#include "ControlProtocol.h"

using namespace std;

ControlProtocol::ControlProtocol(Transmitter &tx, UpdateListener *frameListener)
: m_tx(tx)
, m_frameListener(frameListener)
, m_state(WAIT_FOR_MAGIC1)
{
}

ControlProtocol::~ControlProtocol() {
}

void ControlProtocol::onUpdate(void *arg) {
	Receiver &rx(*(Receiver *)arg);
	char *pframe = reinterpret_cast<char *>(&m_frameIn);
	unsigned char dat;
	int headerSize;

	if (rx.getDataSize() == 0)
		return;

	switch (m_state) {
	case WAIT_FOR_MAGIC1:
		dat = rx.popData();
		if (dat == CF_MAGIC1)
			m_state = WAIT_FOR_MAGIC2;
		break;

	case WAIT_FOR_MAGIC2:
		dat = rx.popData();
		if (dat == CF_MAGIC2)
			m_state = WAIT_FOR_FRAME;
		else
			m_state = WAIT_FOR_MAGIC1;
		break;

	case WAIT_FOR_FRAME:
		headerSize = sizeof(ControlFrame) - sizeof(m_frameIn.magic1) -
			sizeof(m_frameIn.magic2) - sizeof(m_frameIn.data);
		if (rx.getDataSize() >= headerSize) {
			// Read into buffer
			for (int i = 0; i < headerSize; i++)
				pframe[i + 2] = rx.popData(); // + 2 for magic number
			if (m_frameIn.data_size > CF_DATA_MAX) {
				cerr << "Error: " << rx.getName() << " control frame data too larger"
				     << endl;
				m_state = WAIT_FOR_MAGIC1;
			} else {
				m_state = WAIT_FOR_DATA;
				m_dataCnt = 0;
			}
		}
		break;

	case WAIT_FOR_DATA:
		if (m_dataCnt < m_frameIn.data_size) {
			m_frameIn.data[m_dataCnt++] = rx.popData();
			if (m_dataCnt >= m_frameIn.data_size) {
				m_state = WAIT_FOR_MAGIC1;
				// got complete frame
				if (m_frameListener)
					m_frameListener->onUpdate(&m_frameIn);
			}
		}
		break;
	}
}

void ControlProtocol::sendHandshake(int uid, int rateMin, int rateMax) {
	m_frameOut.magic1 = CF_MAGIC1;
	m_frameOut.magic2 = CF_MAGIC2;
	m_frameOut.c2s = 1;
	m_frameOut.ack = 0;
	m_frameOut.tr = 1;
	m_frameOut.uid = uid;

	RateRequest *pDat = reinterpret_cast<RateRequest *>(&m_frameOut.data);
	pDat->min_rate = rateMin;
	pDat->max_rate = rateMax;

	m_frameOut.data_size = sizeof(*pDat);

	char *pframe = reinterpret_cast<char *>(&m_frameOut);

	for (int i = 0; i < m_frameOut.size(); i++)
		m_tx.pushData(pframe[i]);
}
