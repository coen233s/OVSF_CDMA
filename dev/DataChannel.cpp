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
, m_txEnable(false)
, m_rxEnable(false)
, m_tr(tr)
{
    pch.attachReceiver(&m_rx);
    pch.attachTransmitter(&m_tx);
    if (tr)
    {
        string name = getDeviceId() + ".out";
        m_file = fopen(name.c_str(), "wb");
        if (!m_file)
        {
            cout << getDeviceId() << " ERROR: cannot open file: "
                    << name << endl;
        }
    }
    else
    {
        string name = getDeviceId() + ".in";
        m_file = fopen(name.c_str(), "rb");
        if (!m_file)
        {
            cout << getDeviceId() << " ERROR: cannot open file: "
                 << name << endl;
        }
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

    dout("Data channel " << getDeviceId() << " recv ");
    unsigned char data = rx.popData();
    dout(hex << (int)data << dec << endl);
    if (m_tr)
    {
        fwrite(&data, sizeof(data), 1, m_file);
    }
}

void DataChannel::transmit()
{
    if (NULL != m_file) 
    {
        do
        {
            dout("Pushing data: ");
            unsigned char byte[1024];
            size_t size = fread(byte, sizeof(byte[0]), sizeof(byte), m_file); 
            for (size_t i = 0; i < size; ++i)
            {
                dout(byte[i]);
                m_tx.pushData(byte[i]);
            }
            dout(endl);
        }
        while (!feof(m_file));
    }
}

DataChannel::~DataChannel()
{
    m_pch.detachReceiver(&m_rx);
    m_pch.detachTransmitter(&m_tx);
    fclose(m_file);
}
