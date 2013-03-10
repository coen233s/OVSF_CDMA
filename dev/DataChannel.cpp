/*
 * DataChannel.cpp
 *
 *  Created on: Mar 8, 2013
 *      Author: Danke
 */

#include "debug.h"
#include "DataChannel.h"

DataChannel::DataChannel(string &channelId, AbsPhyChannel &pch)
: DeviceBase(channelId)
, m_pch(pch)
, m_tx(channelId + string(".tx"))
, m_rx(channelId + string(".rx"), this)
{
	pch.attachReceiver(&m_rx);
	pch.attachTransmitter(&m_tx);
}

void DataChannel::setTxWalshCode(WHCode &code)
{
	m_tx.setWalshCode(code);
	m_txEnable = true;
}

void DataChannel::setRxWalshCode(WHCode &code)
{
	// WHCode is passed by value
	vector<WHCode> codeSet;
	codeSet.push_back(code);
	m_rx.setWalshCode(codeSet);
	m_rxEnable = true;
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
	DataFrame dframe(*(DataFrame *)arg);
	dout("Data channel " << getDeviceId() << " recv" << endl);
	dout("[" << dframe << "]" << endl);
}

DataChannel::~DataChannel()
{
	m_pch.detachReceiver(&m_rx);
	m_pch.detachTransmitter(&m_tx);
}
