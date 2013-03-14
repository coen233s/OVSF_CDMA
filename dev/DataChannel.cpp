/*
 * DataChannel.cpp
 *
 *  Created on: Mar 8, 2013
 *      Author: Danke
 */

#include <debug.h>
#include <iostream>
#include "DataChannel.h"

using namespace std;

DataChannel::DataChannel(string &channelId, AbsPhyChannel &pch)
: DeviceBase(channelId)
, m_pch(pch)
, m_tx(channelId + string(".tx"))
, m_protData(m_tx, this)
, m_rx(channelId + string(".rx"), &m_protData)
{
    pch.attachReceiver(&m_rx);
    pch.attachTransmitter(&m_tx);
}

void DataChannel::setTxWalshCode(const WHCode &code)
{
    m_tx.setWalshCode(code);
    m_txEnable = true;
}

void DataChannel::setRxWalshCode(const WHCode &code)
{
    m_rx.setWalshCode(code);
    m_rxEnable = true;
}

void DataChannel::addRxWalshCode(const WHCode &code)
{
    m_rx.addWalshCode(code);
    m_rxEnable = true;
}

void DataChannel::removeRxWalshCode(const WHCode &code)
{
    m_rx.removeWalshCode(code);
    m_rxEnable = m_rx.getWalshCode().size() > 0;
}

void DataChannel::removeTx()
{
    m_tx.clearWalshCode();
    m_txEnable = false;
}

void DataChannel::removeRx()
{
    m_rx.clearWalshCode();
    m_rxEnable = false;
}

// Process received data frame
void DataChannel::onUpdate(void *arg)
{
    DataFrame &dframe(*(DataFrame *)arg);

    cout << "Data channel " << getDeviceId() << " recv" << endl;
    cout << "{" << dframe << "}" << endl;
}

DataChannel::~DataChannel()
{
    m_pch.detachReceiver(&m_rx);
    m_pch.detachTransmitter(&m_tx);
}
