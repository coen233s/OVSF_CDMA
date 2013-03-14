/*
 * DataProtocol.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <algorithm>
#include <cstring>
#include <iostream>
#include "DataProtocol.h"

using namespace std;

DataProtocol::DataProtocol(Transmitter &tx, UpdateListener *frameListener)
: m_tx(tx)
, m_frameListener(frameListener)
, m_state(WAIT_FOR_MAGIC1)
{
}

DataProtocol::~DataProtocol() {
}

void DataProtocol::onUpdate(void *arg) {
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
        headerSize = sizeof(DataFrame) - sizeof(m_frameIn.magic1) -
        sizeof(m_frameIn.magic2) - sizeof(m_frameIn.data);
        if (rx.getDataSize() >= headerSize) {
            // Read into buffer
            for (int i = 0; i < headerSize; i++)
                pframe[i + 2] = rx.popData(); // + 2 for magic number
            if (m_frameIn.length > CF_DATA_MAX) {
                cerr << "Error: " << rx.getName() << " control frame data too large"
                        << endl;
                m_state = WAIT_FOR_MAGIC1;
            } else {
                m_state = WAIT_FOR_DATA;
                m_dataCnt = 0;
            }
        }
        break;

    case WAIT_FOR_DATA:
        if (m_dataCnt < m_frameIn.length) {
            m_frameIn.data[m_dataCnt++] = rx.popData();
            if (m_dataCnt >= m_frameIn.length) {
                m_state = WAIT_FOR_MAGIC1;
                // got complete frame
                if (m_frameListener)
                    m_frameListener->onUpdate(&m_frameIn);
            }
        }
        break;
    }
}

void DataProtocol::sendFrame(int fromUid, int toUid, int length, char *pData)
{
    int offset = 0;
    m_frameOut.magic1 = CF_MAGIC1;
    m_frameOut.magic2 = CF_MAGIC2;
    m_frameOut.from_uid = fromUid;
    m_frameOut.to_uid = toUid;

    do {
        m_frameOut.length = min(length, (int)sizeof(m_frameOut.data));
        length -= m_frameOut.length;
        memcpy(m_frameOut.data, &pData[offset], m_frameOut.length);
        offset += m_frameOut.length;

        char *pframe = reinterpret_cast<char *>(&m_frameOut);

        // Transmit
        for (int i = 0; i < m_frameOut.size(); i++)
            m_tx.pushData(pframe[i]);
    } while (length);
}
