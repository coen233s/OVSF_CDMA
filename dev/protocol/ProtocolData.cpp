/*
 * ProtocolData.cpp
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#include <iostream>
#include "ProtocolData.h"

using namespace std;

// dump struct ControlFrame
ostream& operator<<(ostream& os, struct ControlFrame &cf)
{
	os << "c2s:" << (int)cf.c2s << " tr:" << (int)cf.tr
	   << " ack:" << (int)cf.ack << " uid:" << (int)cf.uid
	   << " size:" << (int)cf.data_size;

	if (cf.c2s) {
		RateRequest *pReq = reinterpret_cast<RateRequest *>(&cf.data);
		os << " min_rate:" << (int)pReq->min_rate
		   << " max_rate:" << (int)pReq->max_rate;
	} else {
		CodeAssignment *pCa = reinterpret_cast<CodeAssignment *>(&cf.data);
        os << " code: ";
        for (uint32_t i = 0; i < pCa->length; ++i)
        {
    		os << (int)pCa->code[i] << " ";
        }
	}
    return os;
}
