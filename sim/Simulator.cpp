/*
 * Simulator.cpp
 *
 *  Created on: Mar 5, 2013
 *      Author: Danke
 */

#include "Simulator.h"

Simulator::Simulator() {
}

Simulator::~Simulator() {
}

void Simulator::onTick(int time) {
    for (vector<SimObject *>::iterator it = m_simObjects.begin();
            it != m_simObjects.end();
            it++)
    {
        (*(it))->onTick(time);
    }
}

// Run until shouldStop() returns true
void Simulator::run(bool (*shouldStop)()) {
    m_time = 0;
    while (!shouldStop()) {
        onTick(m_time);
        m_time++;
    }
}

// Run for timeMax time steps
void Simulator::run(int timeLim) {
    for (m_time = 0; m_time < timeLim; m_time++) {
        onTick(m_time);
    }
}
