/*
 * TestTransmit.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <iostream>
#include <string>
#include <vector>
#include <ovsf.h>
#include <phy/Receiver.h>
#include <phy/Transmitter.h>
#include <phy/SimplePhyChannel.h>

using namespace std;

int g_time;
int g_maxTime;

bool testStopCond()
{
    g_time++;
    return g_time > g_maxTime;
}

int main()
{
    string txName("tx");
    string rxName("rx");

    vector<int> codeVec;
    codeVec.push_back(-1);
    codeVec.push_back(1);

    WHCode code(codeVec);
    vector<WHCode *> codeSet;
    codeSet.push_back(&code);

    cout << "Walsh code: ";
    code.print();
    cout << endl;

    SimplePhyChannel pch;
    Transmitter tx(txName);
    Receiver rx(rxName);

    tx.setWalshCode(&code);
    rx.setWalshCode(codeSet);

    pch.attachTransmitter(&tx);
    pch.attachReceiver(&rx);

    // push data
    for (int i = 0; i < 10; i++) {
        cout << "Send data: " << i << endl;
        tx.pushData(i);
    }

    g_time = 0;
    g_maxTime = 10 * 8 * 2;
    pch.run(testStopCond);

    while (rx.hasData())
        cout << "Recv data: " << (int)rx.popData() << endl;

    return 0;
}