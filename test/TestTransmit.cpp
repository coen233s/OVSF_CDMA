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
#include <sim/Simulator.h>

using namespace std;

int main()
{
	Simulator sim;

    string txName("tx");
    string rxName("rx");

    vector<int> codeVec;
    codeVec.push_back(-1);
    codeVec.push_back(1);

    WHCode code(codeVec);
    vector<WHCode> codeSet;
    codeSet.push_back(code);

    cout << "Walsh code: ";
    code.print();
    cout << endl;

    SimplePhyChannel pch;
    Transmitter tx(txName);
    Receiver rx(rxName);

    tx.setWalshCode(code);
    rx.setWalshCode(codeSet);

    pch.attachTransmitter(&tx);
    pch.attachReceiver(&rx);

    // push data
    for (int i = 0; i < 10; i++) {
        cout << "Send data: " << i << endl;
        tx.pushData(i);
    }

    sim.addObject(&pch);
    sim.run(10 * 8 * 2);

    while (rx.hasData())
        cout << "Recv data: " << (int)rx.popData() << endl;

    return 0;
}
