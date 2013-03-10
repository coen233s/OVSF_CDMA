/*
 * SimplePhyChannel.cpp
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#include <iostream>
#include "SimplePhyChannel.h"

using namespace std;

SimplePhyChannel::SimplePhyChannel(int chipRate)
: m_chipRate(chipRate)
{
	cout << "Physical channel is up, chip rate: " <<
			chipRate << "cps" << endl;
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

void SimplePhyChannel::detachReceiver(Receiver *rx) {
	m_Rx.erase(std::remove(m_Rx.begin(), m_Rx.end(),
			rx), m_Rx.end());
}

void SimplePhyChannel::detachTransmitter(Transmitter *tx) {
	m_Tx.erase(std::remove(m_Tx.begin(), m_Tx.end(),
			tx), m_Tx.end());
}

void SimplePhyChannel::onTick(int time) {
	int chipSum = 0;

	// the vector m_Tx may be updated during iteration
	for (vector<Transmitter *>::size_type i = 0;
			i < m_Tx.size(); ++i) {
	    m_Tx[i]->onTick(time);
	    chipSum += m_Tx[i]->getChip();
	}

	// the vector m_Rx may be updated during iteration
	for (vector<Transmitter *>::size_type i = 0;
			i < m_Rx.size(); ++i) {
		m_Rx[i]->setChip(chipSum);
		m_Rx[i]->onTick(time);
	}
}
