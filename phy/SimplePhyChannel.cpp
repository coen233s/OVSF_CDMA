/*
 * SimplePhyChannel.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <iostream>
#include "SimplePhyChannel.h"

using namespace std;

SimplePhyChannel::SimplePhyChannel()
{
}

SimplePhyChannel::~SimplePhyChannel()
{
}

void SimplePhyChannel::attachReceiver(Receiver *rx) {
	m_Rx.push_back(rx);
}

void SimplePhyChannel::attachTransmitter(Transmitter *tx) {
	m_Tx.push_back(tx);
}

void SimplePhyChannel::onTick(int time) {
	int chipSum = 0;

	for (vector<Transmitter *>::iterator it = m_Tx.begin();
			it != m_Tx.end(); ++it) {
	    (*it)->onTick();
	    chipSum += (*it)->getChip();
	}

	for (vector<Receiver *>::iterator it = m_Rx.begin();
				it != m_Rx.end(); ++it) {
		(*it)->setChip(chipSum);
		(*it)->onTick();
	}
}
