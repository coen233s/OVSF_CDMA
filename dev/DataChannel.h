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
#include <stat/Histogram.h>
#include "protocol/ProtocolData.h"
#include <stdio.h>

class BaseStation;
class DataChannel : public DeviceBase,
                    public UpdateListener
{
protected:
	BaseStation *m_pBS;
	int m_id;
public:
    AbsPhyChannel &m_pch;
    Transmitter m_tx;
    Receiver m_rx;
    bool m_txEnable;
    bool m_rxEnable;

    DataChannel(string &channelId, int id, AbsPhyChannel &pch, bool tr);
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

    void transmit();

    // data frame listerner, arg = Receiver*
    virtual void onUpdate(void *arg);

    void setBaseStation(BaseStation *pBS) {
    	m_pBS = pBS;
    }

private:
    FILE* m_file;
    bool m_tr;
};

#endif /* DATACHANNEL_H_ */
