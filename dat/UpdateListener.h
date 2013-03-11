/*
 * UpdateListener.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef UPDATELISTENER_H_
#define UPDATELISTENER_H_

class UpdateListener {
public:
    virtual void onUpdate(void *arg) = 0;
};

#endif /* UPDATELISTENER_H_ */
