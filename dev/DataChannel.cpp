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

DataChannel::DataChannel(string &channelId, AbsPhyChannel &pch, bool tr)
: DeviceBase(channelId)
, m_pch(pch)
, m_tx(channelId + string(".tx"))
, m_rx(channelId + string(".rx"), this)
, m_tr(tr)
{
    pch.attachReceiver(&m_rx);
    pch.attachTransmitter(&m_tx);
    if (tr)
    {
        string name = getDeviceId() + ".out";
        m_file.open(name, ios::out);
    }
    else
    {
        string name = getDeviceId() + ".in";
        m_file.open(name, ios::in);
    }
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
    Receiver &rx(*(Receiver *)arg);

    if (rx.getDataSize() == 0)
        return;

    cout << "Data channel " << getDeviceId() << " recv ";
    unsigned char data = rx.popData();
    cout << hex << (int)data << dec << endl;
    if (m_tr)
    {
        m_file << data;
    }
}

void DataChannel::transmit()
{
    if (m_file.is_open() && !m_file.eof())
    {
        char byte;

        if (m_file.read(&byte, sizeof(byte)))
        {
            m_tx.pushData(byte);
        }
    }
}

DataChannel::~DataChannel()
{
    m_pch.detachReceiver(&m_rx);
    m_pch.detachTransmitter(&m_tx);
    m_file.close();
}
