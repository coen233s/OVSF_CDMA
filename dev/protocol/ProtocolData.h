/*
 * ChannelRequest.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Danke
 */

#ifndef CHANNELREQUEST_H_
#define CHANNELREQUEST_H_

// Need to decide later
enum {
	DR_LOW	= 0,
	DR_MID	= 1,
	DR_HIGH	= 2,
};

#define CF_MAGIC1	0xCC
#define CF_MAGIC2	0xFF

struct ControlFrame {
	unsigned char	magic1;
	unsigned char	magic2;
	char			c2s:1;		// 0 - server to client, 1 - client to server
	char			tr:1;		// 0 - transmit, 1 - receive
	char			ack:1;		// 1 - is ack
	char			uid:3;		// user id
	char			data_size;	// data size below
	unsigned char	data[0];
};

struct RateRequest {
	char			min_rate;	// data rate (min)
	char			max_rate;	// data rate (max)
};

struct CodeAssignment {
	// TODO
};

#endif /* CHANNELREQUEST_H_ */
