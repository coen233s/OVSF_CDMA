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
public:
    virtual void onTick(int time) = 0;
};

#endif /* SIMOBJECT_H_ */
