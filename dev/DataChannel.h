/*
 * DataChannel.h
 *
 *  Created on: Mar 8, 2013
 *      Author: Danke
 */

#ifndef DATACHANNEL_H_
#define DATACHANNEL_H_

#include <ovsf.h>
#include <dat/UpdateListener.h>
#include <dev/DeviceBase.h>
#include <phy/AbsPhyChannel.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>
#include "protocol/ProtocolData.h"

class DataChannel : public DeviceBase,
                    public UpdateListener
{
public:
    AbsPhyChannel &m_pch;
    Transmitter m_tx;
    Receiver m_rx;
    bool m_txEnable;
    bool m_rxEnable;

    DataChannel(string &channelId, AbsPhyChannel &pch);
    ~DataChannel();

    // add Tx code
    void setTxWalshCode(const WHCode &code);

    // set/add/remove Rx code
    void setRxWalshCode(const WHCode &code);
    void addRxWalshCode(const WHCode &code);
    void removeRxWalshCode(const WHCode &code);

    // remove Tx code
    void removeTx();

    // remove Rx code
    void removeRx();

    // check if both tx and rx have been removed
    bool canFreeChannel() {
        return !m_txEnable && !m_rxEnable;
    }

    // data frame listerner, arg = &DataFrame
    virtual void onUpdate(void *arg);
};

#endif /* DATACHANNEL_H_ */
