/*
 * SimObject.h
 *
 *  Created on: Mar 5, 2013
 *      Author: Danke
 */

#ifndef SIMOBJECT_H_
#define SIMOBJECT_H_

// The base class for all simualted objects
class SimObject {
protected:
    bool m_autoRemove;
public:
    SimObject()
    : m_autoRemove(false)
    { }

    virtual ~SimObject(){}

    virtual void onTick(int time) = 0;

    bool isAutoRemove() {
        return m_autoRemove;
    }

    void setAutoRemove(bool remove) {
        m_autoRemove = remove;
    }
};

#endif /* SIMOBJECT_H_ */
