/*
 * Receiver.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_

#include <string>
#include <vector>
#include <ovsf.h>
#include <phy/RxTxBase.h>
#include <dat/BitInQueue.h>
#include <dat/UpdateListener.h>

using namespace std;

class Receiver : public RxTxBase, UpdateListener {
private:
    // Last received chip
    int m_LastChip;

    // Idle count -- channel has been quiet for this number of bits
    int m_idleCount;

    // Set of Walsh codes
    vector<WHCode> m_WalshCode;

    // Walsh code to wait
    WHCode m_WalshCodeToWait;

    // Has walsh code to wait
    bool m_hasWalshToWait;

    // Walsh code index
    vector<int> m_WalshIdx;

    // Walsh code inner product
    vector<int> m_WalshDotProd;

    // Data queue
    BitInQueue m_BitQueue;

    // Update listener
    UpdateListener *m_updateListener;

public:
    Receiver(const string name, UpdateListener *updateListener = 0);
    virtual ~Receiver();
    virtual void onTick(int time);

    void setWalshCode(const vector<WHCode> &newCodes);
    void setWalshCode(const WHCode &newCode);
    void addWalshCode(const WHCode &code);
    void removeWalshCode(const WHCode &code);
    vector<WHCode> &getWalshCode();

    void clearWalshCode() {
        m_WalshCode.clear();
    }

    void setWalshCodeToWait(const WHCode &code) {
        m_WalshCodeToWait = code;
        m_hasWalshToWait = true;
    }

    // Called before onTick(). Sets the chip value.
    // Chip value can be negative, 0, or positive
    void setChip(int chip) {
        m_LastChip = chip;
    }

    bool hasData() {
        return m_BitQueue.hasData();
    }

    int getDataSize() {
        return m_BitQueue.size();
    }

    // Gets the number of bits for which the channel is quiet
    int getIdleCount() {
        return m_idleCount;
    }

    unsigned char popData();
    int peekData(int idx);

    virtual void onUpdate(void *arg);
};

#endif /* RECEIVER_H_ */
