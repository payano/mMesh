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
#include <time.h>

#include <thread>

#include <stdio.h> // REMOVE
#include <unistd.h>

static constexpr int FIND_PARENT_TIMEOUT_MS(1000);
static constexpr int NEIGHBOURS_SZ(5);
static constexpr int BIT_ADDRESS_SZ(3);
static constexpr int NUM_ADDRESSES(7); // 2^3
static constexpr int MSG_BUFFER(10);
static constexpr int MAX_HOPS(12);

//static constexpr int mac_addr_sz(5);
//static constexpr int subnet_sz(4);
static constexpr int NET_COUNT(9);
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


constexpr struct net_address MASTER = {
                                       0, // broadcast
                                       1, // master
                                       0, // gen_addr
                                       0, // unused1
                                       0, // unused2
                                       0, // bits 0
                                       0, // bits 1
                                       0, // bits 2
                                       0, // bits 3
                                       0, // bits 4
                                       0, // bits 5
                                       0, // bits 6
                                       0, // bits 7
                                       0, // bits 8
                                       0, // host addr
};

constexpr struct net_address BROADCAST = {1, // broadcast
                                          0, // master
                                          0, // gen_addr
                                          0, // unused1
                                          0, // unused2
                                          0, // bits 0
                                          0, // bits 1
                                          0, // bits 2
                                          0, // bits 3
                                          0, // bits 4
                                          0, // bits 5
                                          0, // bits 6
                                          0, // bits 7
                                          0, // bits 8
                                          0, // host addr
};

struct neighbour {
	net_address mac;
	uint8_t ping_fail_count;
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
struct broadcast_neighbour_req {
	struct header header;
};

struct broadcast_neighbour_rsp {
	struct header header;
	struct net_address net_address;
};

static constexpr int macaddr_sz(sizeof(struct net_address));
static constexpr int header_sz(sizeof(struct header));
static constexpr int payload_sz(32);

struct message_req {
	struct header header;
	net_address to_addr;
	net_address from_addr;
	uint8_t payload[payload_sz - (header_sz + 2 * macaddr_sz)];
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
	struct message_req             message_req;
	struct message_rsp             message_rsp;
};

struct networkData {
	bool nb_count[NEIGHBOURS_SZ];
	int pairedChildren;
	bool mPaired;
	bool registeredToMaster;
	struct neighbour nbs[NEIGHBOURS_SZ];
	net_address parent;
	net_address mac;
	union mesh_internal_msg *queuedmsgs[MSG_BUFFER];
	int buffer_count;
};

class NetHelper{
public:
	/* this is our simple routing algorithm */
	static const struct net_address *getRouteAddress(const struct networkData *nwd, const struct net_address *dst){
		/* Check nb's */
//		printf("t:%zu, MAC: \n", std::this_thread::get_id());
//		NetHelper::printf_address(&nwd->mac);
//		printf("t:%zu, DST: \n", std::this_thread::get_id());
//		NetHelper::printf_address(dst);
//		for(int i = 0; i < nwd->pairedChildren; ++i){
//		printf("t:%zu, Child: \n", std::this_thread::get_id());
//		NetHelper::printf_address(&nwd->nbs[i].mac);
//		}

		if(1 == nwd->mac.master) {
			for(int i = 0 ; i < NEIGHBOURS_SZ; ++i){
				if(nwd->nbs[i].mac.nbs[0].net == dst->nbs[0].net) {
					return &nwd->nbs[i].mac;
				}
			}
		}
//		printf("t:%zu, NOT MASTER\n", std::this_thread::get_id());
//		printf("t:%zu, PAIRED CHILDS: %d\n", std::this_thread::get_id(), nwd->pairedChildren);
		for(int i = 0; i < nwd->pairedChildren; ++i) {
//			printf("t:%zu ,CHILD: ", std::this_thread::get_id());
//			NetHelper::printf_address(&nwd->nbs[i].mac);
			bool childOf = NetHelper::isChildOf(&nwd->nbs[i].mac, dst);
//			printf("t:%zu, Is child of: %d\n", std::this_thread::get_id(), childOf);
			if(true == childOf) {
//				printf("t:%zu, 1RET CHILD\n", std::this_thread::get_id());
				return &nwd->nbs[i].mac;
			}
			if(true == NetHelper::compare_net_address(dst, &nwd->nbs[i].mac))
			{
//				printf("t:%zu, 2RET CHILD\n", std::this_thread::get_id());
				return dst;
			}
		}
//		printf("t:%zu, RET PARENT\n", std::this_thread::get_id());
		return &nwd->parent;
	}

	static bool isChildOf(const struct net_address *parent, const struct net_address *child){
		if(parent->master) return true; // all is childs to master

		for(int i = 0; i < NET_COUNT; ++i) {
			if(parent->nbs[i].net == child->nbs[i].net) continue;
			if(parent->nbs[i].net == 0x0) {return true;}
			break;
		}
		return false;
	}
	static int queue_sz(struct networkData *nwd) { return nwd->buffer_count; }
	static void queue_clear(struct networkData *nwd){
		for(int i = nwd->buffer_count-1; i >= 0; --i){
			delete nwd->queuedmsgs[i];
		}
		nwd->buffer_count = 0;
	}

	static int queue_add(struct networkData *nwd, union mesh_internal_msg *msg){
		if(nwd->buffer_count >= MSG_BUFFER) return -1;
		union mesh_internal_msg *add_msg = new mesh_internal_msg;

		copy_internal_msg(add_msg, msg);
		nwd->queuedmsgs[nwd->buffer_count++] = add_msg;
		return 0;

	}

	static int queue_get(struct networkData *nwd, union mesh_internal_msg **msg){
//		printf("queue_get ptr: %p\n",);
		*msg = nwd->queuedmsgs[nwd->buffer_count-1];
		nwd->buffer_count--;
		return 0;
	}

	static bool compare_net_address(const struct net_address *one, const struct net_address *two)
	{
		if(one->broadcast != two->broadcast) return false;
		if(one->master != two->master) return false;
		if(one->gen_addr != two->gen_addr) return false;
		for(int i = 0 ; i < NET_COUNT; ++i) {
			if(one->nbs[i].net != two->nbs[i].net) return false;
		}
		if(one->host_addr != two->host_addr) return false;

		return true;
	}

	static void copy_net_address(struct net_address *to, const struct net_address *from)
	{
		to->broadcast = from->broadcast;
		to->master = from->master;
		to->gen_addr = from->gen_addr;
		for(int i = 0 ; i < NET_COUNT; ++i) {
			to->nbs[i].net = from->nbs[i].net;
		}
		to->host_addr = from->host_addr;
	}

	static void clear_net_address(struct net_address *address) {
		address->broadcast = 0;
		address->master = 0;
		address->gen_addr = 0;
		address->unused1 = 0;
		address->unused2 = 0;
		for(int i = 0 ; i < NET_COUNT; ++i) {
			address->nbs[i].net = 0;
		}
		address->host_addr = 0;
	}

	static void init_networkData(struct networkData *nwd){
		clear_net_address(&nwd->mac);
		clear_net_address(&nwd->parent);
		nwd->pairedChildren = 0;
		nwd->registeredToMaster = false;
		nwd->mPaired = false;
		nwd->buffer_count = 0;

		for(int i = 0 ; i < NEIGHBOURS_SZ; ++i) {
			clear_net_address(&nwd->nbs[i].mac);
			nwd->nbs[i].ping_fail_count = 0;
			nwd->nb_count[i] = false;
		}
	}

	static void generate_temporary_address(struct net_address *address) {
		address->broadcast = 0;
		address->master = 0;
		address->gen_addr = 1;
		for(int i = 0 ; i < NET_COUNT; ++i) {
			address->nbs[i].net = generate_number(NUM_ADDRESSES);
		}
		address->host_addr = generate_number(NUM_ADDRESSES);
	}

	static int generate_child_address(struct networkData *networkData, struct net_address *address) {
		clear_net_address(address);
		int ret;
		int child_addr = -1;
		for(int i = 0; i < NEIGHBOURS_SZ; ++i){
			if(networkData->nb_count[i] == false) {
				// Found a free slot
				child_addr = i;
				break;
			}
		}

		// No addresses free.. here we might need to release one.
		if(child_addr < 0) {return -1;}
		// Assign it
		networkData->nb_count[child_addr] = true;
		// When using it we want to start with number 1 not 0
		child_addr++;
		networkData->pairedChildren++;
		// Get new address
		ret = getNewChildAddress(&networkData->mac, address, child_addr);
		if(ret) {
			return -1;
		}
		// Add it to parent neighbour list
		copy_net_address(&networkData->nbs[child_addr-1].mac, address);
		return 0;
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
		case MSGNO::MESSAGE_REQ: return "MESSAGE_REQ";
		case MSGNO::MESSAGE_RSP: return "MESSAGE_RSP";
		}
		return "MSGNO DOESN'T EXIST";
	}
private:
	static int copy_internal_msg(union mesh_internal_msg *to,
	                             union mesh_internal_msg *from) {
		int sz = sizeof(*to);
		void *void_to = static_cast<void*>(to);
		void *void_from = static_cast<void*>(from);
		uint8_t *uint8_to = static_cast<uint8_t *>(void_to);
		uint8_t *uint8_from = static_cast<uint8_t *>(void_from);

		for(int i = 0 ; i < sz ; ++i){
			uint8_to[i] = uint8_from[i];
		}
		return 0;
	}

	static int getNewChildAddress(const struct net_address *parent, struct net_address *child, const int childId) {
		if(parent->master) {
			// Easy one
			child->nbs[0].net = childId;
			return 0;
		}
		// here we need to find the parent, also copying the bytes from parent
		for(int i = 0 ; i < NET_COUNT ; ++i) {
			child->nbs[i].net = parent->nbs[i].net;
			if(parent->nbs[i].net == 0) {
				child->nbs[i].net = childId;
				return 0;
			}
		}
		return -1;
	}
	static uint8_t generate_number(int max_number){
		return rand() % NUM_ADDRESSES;
	}
	NetHelper(){};

};

