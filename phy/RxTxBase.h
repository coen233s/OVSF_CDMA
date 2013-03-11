/*
 * RxTxBase.h
 *
 *  Created on: Feb 28, 2013
 *      Author: Danke
 */

#ifndef RXTXBASE_H_
#define RXTXBASE_H_

#include <string>
#include <NamedObject.h>

using namespace std;

class RxTxBase : public NamedObject {
public:
    RxTxBase(const string &name);
    virtual ~RxTxBase();

    // Simulate one time step (one chip period).
    void onTick();
};

#endif /* RXTXBASE_H_ */
