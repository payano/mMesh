/*
MIT License

Copyright (c) 2020 Johan Svensson

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <stdint.h>
#define DEBUG 1

// linux randomizer
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h> // REMOVE

#include "Constants.h"

// 5 Bytes = 40 bits in total
// Net address is 4 Bytes = 32 Bits
struct net_address {
	uint8_t broadcast : 1; /* Broadcast packet */
	uint8_t master    : 1;
	uint8_t gen_addr  : 1; // Generated address, when not assigned to network
	uint8_t unused1   : 1;
	uint8_t unused2   : 1;
	struct net_bits {
		uint8_t net : 3;
	} nbs[NET_COUNT];
	uint8_t host_addr;
}__attribute__ ((__packed__));

static constexpr int macaddr_sz(sizeof(struct net_address));

static constexpr struct net_address MASTER = {
                                       0, // broadcast
                                       1, // master
                                       0, // gen_addr
                                       0, // unused1
                                       0, // unused2
                                       {
                                        {0}, // bits 0
                                        {0}, // bits 1
                                        {0}, // bits 2
                                        {0}, // bits 3
                                        {0}, // bits 4
                                        {0}, // bits 5
                                        {0}, // bits 6
                                        {0}, // bits 7
                                        {0}  // bits 8
                                       },
                                        0 // host addr
};

static constexpr struct net_address BROADCAST = {1, // broadcast
                                          0, // master
                                          0, // gen_addr
                                          0, // unused1
                                          0, // unused2
                                          {
                                           {0}, // bits 0
                                           {0}, // bits 1
                                           {0}, // bits 2
                                           {0}, // bits 3
                                           {0}, // bits 4
                                           {0}, // bits 5
                                           {0}, // bits 6
                                           {0}, // bits 7
                                           {0}  // bits 8
                                          },
                                           0 // host addr
};

struct node_data {
	net_address mac;
	int32_t keepalive_count;
	bool connected;
};


/* this is msgno in header */

enum MSGNO {
	INVALID       = 0x00,
	BROADCAST_ASSOCIATE_REQ,
	BROADCAST_ASSOCIATE_RSP,
	BROADCAST_NEIGHBOUR_REQ,
	BROADCAST_NEIGHBOUR_RSP,
	NETWORK_ASSIGNMENT_REQ,
	NETWORK_ASSIGNMENT_RSP,
	REGISTER_TO_MASTER_REQ,
	REGISTER_TO_MASTER_RSP,
	PING_PARENT_REQ,
	PING_PARENT_RSP,
	PING_NEIGHBOUR_REQ,
	PING_NEIGHBOUR_RSP,
	DISCONNECT_CHILD_REQ,
	DISCONNECT_CHILD_RSP,
	MESSAGE_REQ,
	MESSAGE_RSP,
};

enum STATUS {
	OK = 0x0,
	FAIL = 0x1,
};
//__attribute__ ((__packed__))

struct header {
	MSGNO msgno;
	uint8_t hop_count : 4;
	uint8_t unused11 : 4;
//	uint8_t pktno : 4;
//	uint8_t noofpkts : 4;
//	mac_addr from_addr;
};
static constexpr int header_sz(sizeof(struct header));


/* Try to join a network */
struct broadcast_associate_req {
	struct header header;
	struct net_address from_addr;
};

struct broadcast_associate_rsp {
	struct header header;
	struct net_address parent_address;
};

struct network_assignment_req {
	struct header header;
	struct net_address from_address;

};

struct network_assignment_rsp {
	struct header header;
	struct net_address new_address;
	struct net_address parent;

};

/* register_to_master_req will be used in a application outside the
 * mesh network.
 * */
struct register_to_master_req {
	struct header header;
	struct net_address host_addr;
	uint64_t unique_id;
};

struct register_to_master_rsp {
	struct header header;
	struct net_address destination;
	STATUS status;

};

struct ping_parent_req {
	struct header header;
	struct net_address from;
	struct net_address to;
};

struct ping_parent_rsp {
	struct header header;
	struct net_address from;
	struct net_address to;
	STATUS status;
};

struct disconnect_child_req {
	struct header header;
	struct net_address from;
	struct net_address to;
};

struct disconnect_child_rsp {
	struct header header;
	struct net_address from;
	struct net_address to;
	STATUS status;
};

struct broadcast_neighbour_req {
	struct header header;
	struct net_address from;
};

struct broadcast_neighbour_rsp {
	struct header header;
	struct net_address net_address;
};

struct ping_neighbour_req {
	struct header header;
	struct net_address from;
	struct net_address to;
};

struct ping_neighbour_rsp {
	struct header header;
	struct net_address from;
	struct net_address to;
};

struct message_req {
	struct header header;
	net_address to_addr;
	net_address from_addr;
	uint8_t payload[PAYLOAD_SZ - (header_sz + 2 * macaddr_sz)];
};

struct message_rsp {
	struct header header;
};

union mesh_internal_msg {
	struct header                  header;
	struct broadcast_associate_req associate_req;
	struct broadcast_associate_rsp associate_rsp;
	struct broadcast_neighbour_req neighbour_req;
	struct broadcast_neighbour_rsp neighbour_rsp;
	struct network_assignment_req  assignment_req;
	struct network_assignment_rsp  assignment_rsp;
	struct register_to_master_req  reg_master_req;
	struct register_to_master_rsp  reg_master_rsp;
	struct ping_parent_req         ping_parent_req;
	struct ping_parent_rsp         ping_parent_rsp;
	struct ping_neighbour_req      ping_neighbour_req;
	struct ping_neighbour_rsp      ping_neighbour_rsp;
	struct disconnect_child_req    disconnect_child_req;
	struct disconnect_child_rsp    disconnect_child_rsp;
	struct message_req             message_req;
	struct message_rsp             message_rsp;
};

template <typename T>
void copy_data(T *to, const T *from, int sz) {
	void *void_to = static_cast<void*>(to);
	const void *void_from = static_cast<const void*>(from);
	uint8_t *to_d = static_cast<uint8_t*>(void_to);
	const uint8_t *from_d = static_cast<const uint8_t*>(void_from);
	for(int i = 0 ; i < sz ; ++i){
		*to_d = *from_d;
		++to_d;
		++from_d;
	}
}

template <typename T>
void mem_clr(T *buffer, int sz) {
	void *void_buffer = static_cast<void*>(buffer);
	uint8_t *buffer_d = static_cast<uint8_t*>(void_buffer);
	for(int i = 0 ; i < sz ; ++i){
		*buffer_d = 0;
		++buffer_d;
	}
}

template <typename T>
int cmp_data(const T *first, const T *second, int sz) {
	const void *void_first = static_cast<const void*>(first);
	const void *void_second = static_cast<const void*>(second);
	const uint8_t *first_d = static_cast<const uint8_t*>(void_first);
	const uint8_t *second_d = static_cast<const uint8_t*>(void_second);
	for(int i = 0 ; i < sz ; ++i){
		if(*first_d != *second_d) return -1;
		++first_d;
		++second_d;
	}
	return 0;
}

class NetHelper{
public:

	/* Should be moved to a class*/
	static bool check_timer_zero(struct node_data *node){
		return node->keepalive_count == 0 ? true : false;
	}

	/* Should be moved to a class*/
	static bool checkConnected(struct node_data *node){
		return node->connected == true ? true : false;
	}

//	static bool compare_net_address(const struct net_address *one, const struct net_address *two)
//	{
//		return cmp_data(one, two, sizeof(*one)) == 0 ? true : false;
//	}
//
//	static void copy_net_address(struct net_address *to, const struct net_address *from)
//	{
//		copy_data(to, from, sizeof(*from));
//	}
//
//	static void clear_net_address(struct net_address *address) {
//		mem_clr(address, sizeof(*address));
//	}

	static void generate_temporary_address(struct net_address *address) {
		address->broadcast = 0;
		address->master = 0;
		address->gen_addr = 1;
		for(int i = 0 ; i < NET_COUNT; ++i) {
			address->nbs[i].net = generate_number(NUM_ADDRESSES);
		}
		address->host_addr = generate_number(NUM_ADDRESSES);
	}

	static void printf_address(const struct net_address *address) {
		printf("broadcast: %d, "
				"master: %d, "
				"generated: %d, "
				"net: 0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x:0x%01x "
				"host addr: 0x%02x\n",
				address->broadcast,
				address->master,
				address->gen_addr,
				address->nbs[0].net,
				address->nbs[1].net,
				address->nbs[2].net,
				address->nbs[3].net,
				address->nbs[4].net,
				address->nbs[5].net,
				address->nbs[6].net,
				address->nbs[7].net,
				address->nbs[8].net,
				address->host_addr);
	}

	static const char *getMsgno(MSGNO msgno){
		switch(msgno){
		case MSGNO::INVALID: return "INVALID";
		case MSGNO::BROADCAST_ASSOCIATE_REQ: return "BROADCAST_ASSOCIATE_REQ";
		case MSGNO::BROADCAST_ASSOCIATE_RSP: return "BROADCAST_ASSOCIATE_RSP";
		case MSGNO::BROADCAST_NEIGHBOUR_REQ: return "BROADCAST_NEIGHBOUR_REQ";
		case MSGNO::BROADCAST_NEIGHBOUR_RSP: return "BROADCAST_NEIGHBOUR_RSP";
		case MSGNO::NETWORK_ASSIGNMENT_REQ: return "NETWORK_ASSIGNMENT_REQ";
		case MSGNO::NETWORK_ASSIGNMENT_RSP: return "NETWORK_ASSIGNMENT_RSP";
		case MSGNO::REGISTER_TO_MASTER_REQ: return "REGISTER_TO_MASTER_REQ";
		case MSGNO::REGISTER_TO_MASTER_RSP: return "REGISTER_TO_MASTER_RSP";
		case MSGNO::PING_PARENT_REQ: return "PING_PARENT_REQ";
		case MSGNO::PING_PARENT_RSP: return "PING_PARENT_RSP";
		case MSGNO::DISCONNECT_CHILD_REQ: return "DISCONNECT_CHILD_REQ";
		case MSGNO::DISCONNECT_CHILD_RSP: return "DISCONNECT_CHILD_RSP";
		case MSGNO::MESSAGE_REQ: return "MESSAGE_REQ";
		case MSGNO::MESSAGE_RSP: return "MESSAGE_RSP";
		case PING_NEIGHBOUR_REQ: return "PING_NEIGHBOUR_REQ";
		case PING_NEIGHBOUR_RSP: return "PING_NEIGHBOUR_RSP";
		}
		return "MSGNO DOESN'T EXIST";
	}


	static uint8_t generate_number(int max_number){
		(void) max_number;
		return rand() % NUM_ADDRESSES;
	}
	NetHelper(){};

};

