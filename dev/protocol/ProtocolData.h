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

// Need to decide later
enum {
	DR_LOW	= 0,
	DR_MID	= 1,
	DR_HIGH	= 2,
};

#define CF_MAGIC1	0xCC
#define CF_MAGIC2	0xFF

#define CF_DATA_MAX	256

#ifdef WIN32
#pragma pack(push)
#pragma pack(2)
#endif
struct ControlFrame {
	uint8_t			magic1;
	uint8_t			magic2;
	uint8_t			c2s:1;		// 0 - server to client, 1 - client to server
	uint8_t			tr:1;		// 0 - transmit, 1 - receive
	uint8_t			ack:1;		// 1 - is ack
	uint16_t		uid;		// user id
	uint8_t			data_size;	// data size (starting from next byte)
	uint8_t			data[CF_DATA_MAX];
} 
#ifdef WIN32
;
#else
__attribute__((packed));
#endif

struct RateRequest {
	uint8_t			min_rate;	// data rate (min)
	uint8_t			max_rate;	// data rate (max)
} 
#ifdef WIN32
;
#else
__attribute__((packed));
#endif

struct CodeAssignment {
	// TODO
};

#ifdef WIN32
#pragma pack(pop)
#endif

ostream& operator<<(ostream& os, struct ControlFrame &cf);

#endif /* CHANNELREQUEST_H_ */
