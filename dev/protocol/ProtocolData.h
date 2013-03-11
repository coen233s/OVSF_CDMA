/*
 * ChannelRequest.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef CHANNELREQUEST_H_
#define CHANNELREQUEST_H_

#include <stdint.h>
#include <iostream>

using namespace std;

// Data rates in bps
enum {
    RATE_1200 = 1200,			// IS-95 rate set 1
    RATE_2400 = 2400,			// IS-95 rate set 1
    RATE_4800 = 4800,			// IS-95 rate set 1
    RATE_9600 = 9600,			// IS-95 rate set 1
    RATE_19200 = 19200,			// Extended
    RATE_38400 = 38400,			// Extended
    RATE_76800 = 76800,			// Extended
    RATE_153600 = 153600,		// Extended
    RATE_307K = 307200,			// Extended
    RATE_614K = 614400,			// Extended
    RATE_1M2 = 1228800,			// Extended
    RATE_2M4 = 2457600,			// Extended
    RATE_4M9 = 4915200,			// Extended
    RATE_9M8 = 9830400,			// Extended
    RATE_19M = 19660800,		// Extended
    RATE_39M = 39321600,		// Extended

    RATE_MIN = RATE_1200,
    RATE_MAX = RATE_39M,
};

#define CF_MAGIC1	0xCC
#define CF_MAGIC2	0xFF

#define CF_DATA_MAX	256			// Control frame max size
#define DF_DATA_MAX	256			// Data frame max size

#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
struct ControlFrame {
    uint8_t			magic1;     // byte 0
    uint8_t			magic2;     // byte 1
    uint8_t			c2s:1;		// 0 - server to client, 1 - client to server
    uint8_t			tr:1;		// 0 - transmit, 1 - receive
    uint8_t			ack:1;		// 1 - is ack
    uint8_t			req:1;		// 1 - request, 0 - tear down
    uint16_t		uid;		// user id, byte 3,4
    uint8_t			data_size;	// data size, byte 5
    uint8_t			data[CF_DATA_MAX]; // Payload

    ControlFrame():
        magic1(CF_MAGIC1),
        magic2(CF_MAGIC2),
        c2s(0),
        tr(0),
        ack(0),
        req(1),
        data_size(0)
    {
    }

    uint16_t size()
    {
        uint16_t frameSize = sizeof(*this) - sizeof(data) + data_size;
        return frameSize;
    }
} 
#ifdef WIN32
;
#else
__attribute__((packed));
#endif

struct RateRequest {
    uint32_t			min_rate;	// data rate (min)
    uint32_t			max_rate;	// data rate (max)
} 
#ifdef WIN32
;
#else
__attribute__((packed));
#endif

struct CodeAssignment {
    uint8_t length;
    uint8_t code[1];
    CodeAssignment() : length(0) {}
};

#ifdef WIN32
#pragma pack(pop)
#endif

// This is a logical data structure used to pass data between
// DataChannel and Receiver/Receiver
struct DataFrame {
    uint16_t length;
    uint8_t data[DF_DATA_MAX];
};

ostream& operator<<(ostream& os, struct ControlFrame &cf);
ostream& operator<<(ostream& os, struct DataFrame &cf);

#endif /* CHANNELREQUEST_H_ */
