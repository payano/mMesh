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

/*
 * Mesh.h
 *
 *  Created on: Feb 12, 2020
 *      Author: evsejho
 */

#ifndef SRC_MESH_H_
#define SRC_MESH_H_

#include <stdint.h>
/* should be removed later */
#include <stdio.h>
#include <iostream>
#include <thread>         // std::thread

/* // */
#include "../network/NetworkInterface.h"
#include "../network/NetworkInterfaceSubscriber.h"
#include "MeshMessagePublisher.h"
#include "MeshMessageSubscriber.h"
#include "../DataTypes.h"
namespace mesh {

/* Mac List
 * 0 = Master
 * */

/*
 *  NRF MODULE FRAME
 *  B = Byte
 *  b = Bit
 *                                    FRAME
 *  -------------------------------------------------------------------------
 *  |           |           |                       |             |         |
 *  |Preamble 1B|Adress 3-5B|Packet Control Field 9b|Payload 0-32B|CRC 1-2B |
 *  |           |           |                       |             |         |
 *  -------------------------------------------------------------------------
 *
 *             Packet Control Field
 *  ------------------------------------------
 *  |                   |        |           |
 *  |Payload length 6bit|PID 2bit|NO_ACK 1bit|
 *  |                   |        |           |
 *  ------------------------------------------
 * The packet control field contains a 6 bit payload length field,
 * a 2 bit PID (Packet Identity) field and a 1 bit NO_ACK flag.
 *
 *
 *
 * MAC ADDRESSES:
 *  0xFF 0XFF 0XFF 0XFF 0XFF 0XFF
 *
 *  Broadcast: 0xff 0xff 0xff 0xff 0xff 0xff
 *  Then
 *  First group with master is:
 *  0_XXX 0xff 0xff 0xff 0xff 0xff
 *
 *  Master is: 0x7f 0xff 0xff 0xff 0xff 0xff
 *  Neighbours 0: 0b00110 1111 = [0x6f - 0x0f] 0xff 0xff 0xff 0xff 0xff
 *
 *  One neighbour:
 *  Neighbour: 0x6f 0xff 0xff 0xff 0xff 0xff
 *
 *  Neighour "one" will also have an adress of:
 *  0x6e 0xff 0xff 0xff 0xff 0xff
 *  A sibling to "one" neighbour will have an adress:
 *  0x06f
 */


static constexpr int MAX_NAME(50);
static constexpr int CHILD_COUNT(5);

/*! \brief Mesh network topology class.
 *
 *  Implementation of Mesh
 */

/* this can be in cpp */
enum STATE {
	STATE_NONE = 0,
	STATE_INIT,
	STATE_MASTER,
	STATE_STARTING,
	STATE_STARTED,
	STATE_STOPPING,
	STATE_STOPPED,
	STATE_ERROR,
};
enum STARTING_STATE {
	STARTING_SEEKING_PARENT,
	STARTING_CHOOSING_PARENT,
	STARTING_REGISTER_TO_PARENT,
	STARTING_WAITING_FOR_PARENT,
	STARTING_REGISTER_TO_MASTER,
	STARTING_WAITING_FOR_MASTER,
};

enum STARTED_STATE {
	STARTED_IDLE,
};

struct stateData {
	enum STATE topState;
	enum STARTING_STATE starting_state;

};

using namespace network;
class Mesh : public MeshMessagePublisher, NetworkInterfaceSubscriber {
private:

	struct networkData network;  /*!< Detailed description after the member */
	NetworkInterface *nw;
	char name[MAX_NAME];
	uint8_t childs[CHILD_COUNT];
	bool mSetMaster;

	struct stateData statedata;
	void initStateMachine(){
		statedata.topState = STATE::STATE_INIT;
	}

	/* in linux this will not be a callback function
	 * this is something to simulate the microcontroller
	*/
	void timerCallback(int ms){
		timerStarted = true;
		usleep(ms*1000);
		timerDone = true;
		timerStarted = false;
	}

	bool timerStarted;
	bool timerDone;
	std::thread *mThread;
	void armTimer(int ms) {
		/* Only one at the time */
		if(timerStarted) return;

		/* this should access a hw timer */
		timerDone = false;
		mThread = new std::thread(&Mesh::timerCallback, this, ms);

	}
	void stateMachine() {
		switch(statedata.topState) {
		case STATE_INIT:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_init();
			break;
		case STATE_MASTER:
//			printf("STATE_MASTER\n");
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			break;
		case STATE_STARTING:
//			printf("%s: FUNCTION: %s, LINE: %d\n", getName(), __FUNCTION__, __LINE__);
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_starting();
			break;
		case STATE_STARTED:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_started();
			break;
		case STATE_STOPPING:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_stopping();
			break;
		case STATE_STOPPED:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_stopped();
			break;
		case STATE_NONE:
		case STATE_ERROR:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_error();
			break;
		}
	}
	void sm_init() {
		// setup network hardware, ...
		// Generate address

		// when setup is done.
		if(mSetMaster) {
			statedata.topState = STATE::STATE_MASTER;
			return;
		}

		statedata.topState = STATE::STATE_STARTING;
		statedata.starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;

	}

	void sm_starting_seeking_parent() {
		armTimer(100); /* 100 ms */
		statedata.starting_state = STARTING_STATE::STARTING_CHOOSING_PARENT;
		doAssociateReq();
	}

	void sm_starting_choosing_parent() {
		/* Here we also get incoming broadcast messages */
		// Check incoming msg, sorting might be good.
		if(!timerDone) return;
//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);

		if(0 == NetHelper::queue_sz(&network)) {
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			statedata.starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
			armTimer(100); /* 100 ms */
			return;
		}
		// Choose the right one to associate with.
		// We take the first one and discard the rest.
		union mesh_internal_msg *queue_msg;
		NetHelper::queue_get(&network, &queue_msg);

		if(MSGNO::BROADCAST_ASSOCIATE_RSP != queue_msg->header.msgno){
			// This is error
			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			delete queue_msg;
		}

		NetHelper::queue_clear(&network);
		struct broadcast_associate_rsp *queue_rsp = &queue_msg->associate_rsp;

		union mesh_internal_msg rsp;
		rsp.assignment_req.header.msgno = MSGNO::NETWORK_ASSIGNMENT_REQ;
		NetHelper::copy_net_address(&rsp.associate_req.from_addr, &network.mac);
		nw->sendto(&queue_rsp->parent_address, &rsp);
		delete queue_msg;

//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
		// Done
		statedata.starting_state = STARTING_STATE::STARTING_REGISTER_TO_PARENT;
	}

	void sm_starting_register_to_parent() {
		armTimer(100); /* 100 ms */
		statedata.starting_state = STARTING_STATE::STARTING_WAITING_FOR_PARENT;
	}

	void sm_starting_waiting_for_parent() {
		if(!timerDone) return;

		if(0 == NetHelper::queue_sz(&network))
		{
			// No answer, restart.
			statedata.starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			return;
		}

		union mesh_internal_msg *queue_msg;
		NetHelper::queue_get(&network, &queue_msg);
		NetHelper::queue_clear(&network);
		if(MSGNO::NETWORK_ASSIGNMENT_RSP != queue_msg->header.msgno){
			// Error
			statedata.starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			delete queue_msg;
			return;
		}

//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
		struct network_assignment_rsp *msg = &queue_msg->assignment_rsp;
		NetHelper::copy_net_address(&network.parent, &msg->parent);
		NetHelper::copy_net_address(&network.mac, &msg->new_address);
		nw->setAddr(&network.mac);
		network.mPaired = true;

		delete queue_msg;
		statedata.starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;

		// Change state
	}

	void sm_starting_register_to_master() {
//		printf("%s: %s\n", this->name, __FUNCTION__);
		armTimer(100); /* 100 ms */
		doRegisterReq();
		statedata.starting_state = STARTING_STATE::STARTING_WAITING_FOR_MASTER;
	}

	void sm_starting_waiting_for_master() {
		if(!timerDone) return;

		if(0 == NetHelper::queue_sz(&network))
		{
			// No answer, restart.
			statedata.starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
			printf("%s: FUNCTION: %s, LINE: %d\n", getName(), __FUNCTION__, __LINE__);
			return;
		}

		union mesh_internal_msg *queue_msg;
		NetHelper::queue_get(&network, &queue_msg);
		NetHelper::queue_clear(&network);
		if(MSGNO::REGISTER_TO_MASTER_RSP != queue_msg->header.msgno){
			// Error
			statedata.starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
			delete queue_msg;
			return;
		}

		struct register_to_master_rsp *msg = &queue_msg->reg_master_rsp;
		network.registeredToMaster = msg->status == STATUS::OK ? true : false;
		delete queue_msg;

		if(!network.registeredToMaster) {
			statedata.starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
			return;
		}

		statedata.topState = STATE::STATE_STARTED;
	}


	void sm_starting() {
		// try to get online.
		switch(statedata.starting_state) {
		case STARTING_SEEKING_PARENT:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_starting_seeking_parent();
			break;
		case STARTING_CHOOSING_PARENT:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_starting_choosing_parent();
			break;
		case STARTING_REGISTER_TO_PARENT:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_starting_register_to_parent();
			break;
		case STARTING_WAITING_FOR_PARENT:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			sm_starting_waiting_for_parent();
			break;
		case STARTING_REGISTER_TO_MASTER:
			sm_starting_register_to_master();
			break;
		case STARTING_WAITING_FOR_MASTER:
			sm_starting_waiting_for_master();
			break;
		}
	}

	void sm_started() {
//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
	}
	void sm_stopping() {}
	void sm_stopped() {}
	void sm_error() {}


protected:

public:
	Mesh(NetworkInterface *nw) : nw(nw) {
		NetHelper::init_networkData(&network);
		nw->registerSubscriber(this);
		initStateMachine();
		timerStarted = false;
		timerDone = false;
		mSetMaster = false;
		for(int i = 0 ; i < MAX_NAME; ++i) {name[i] = 0;}
		for(int i = 0 ; i < CHILD_COUNT; ++i){childs[i] = 0x0;}
	}
	virtual ~Mesh() {
		delete nw;
	}

	void setMaster() {
		char name[] = "master";
		setPaired(true);
		mSetMaster = true;
		setName(name);
		setTemporaryMacAddr(&MASTER);
	}

	int run() {
//		printf("name: %s\n", name);
//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
		stateMachine();
		return 0;
	}

	void setName(char *name)
	{
		char *thisName = this->name;
		while(*name != '\0') {
			*thisName = *name;
			thisName++;
			name++;
		}
	}

	void getParent(struct net_address *addr){
		NetHelper::copy_net_address(addr, &network.parent);
	}

	bool setTemporaryMacAddr(const struct net_address *mac){
		NetHelper::copy_net_address(&network.mac, mac);
		nw->setAddr(mac);
//		printf("master:\n");
		return true;
	}

	void init(){
//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
	}

	int getSubnetToChild(struct net_address *address) {
		/*
		 * Find the address backwards. For example
		 * An subnet can be 0x00 0x00 0x00 0x10 0x00
		 * then the subnet to child will be:
		 * 0x00 0x00 0x01 0x10 0x00, and child of that child, the
		 * subnet will be:
		 * 0x00 0x00 0x11 0x10 0x00
		 */
		int ret = 0;
		// init subnet
		NetHelper::clear_net_address(address);

		ret = NetHelper::generate_child_address(&network, address);
		if(ret) {
//			printf("%s: new child address failed: %d\n", getName(), __LINE__);
			return -1;
		}

#if DEBUG
//		printf("%s\n", __FUNCTION__);
//		NetHelper::printf_address(address);
#endif
		return 0;
	}

	int getBestRouteAddr(struct net_address *dest) {
		/* Need to figure out which direction the packet is going,
		 * towards master or child.
		 */

		return 0;
	}
	void handle_associate_rsp(union mesh_internal_msg *msg) {
		/* Okay, someone responded. There is a network to be
		 * associated with, ask the responder for a address.
		 */

		NetHelper::queue_add(&network, msg);
		return;

//		union mesh_internal_msg req;
//		req.header.msgno = MSGNO::NETWORK_ASSIGNMENT_REQ;
//
//		NetHelper::copy_net_address(&req.assignment_req.from_address, &network.mac);
//		printf("from address:");
//		NetHelper::printf_address(&req.assignment_req.from_address);
//		printf("to address:");
//		NetHelper::printf_address(&msg->parent_address);

//		nw->sendto(&msg->parent_address, &req);

#if DEBUG
//		printf("%s\n", __FUNCTION__);
//		printf("Parent Address: ");
//		NetHelper::printf_address(&msg->parent_address);
#endif
	}

	void handle_associate_req(struct broadcast_associate_req *msg){
//		printf("%s\n", __FUNCTION__);
		union mesh_internal_msg rsp;
		rsp.header.hop_count = 0;
		rsp.header.msgno = MSGNO::BROADCAST_ASSOCIATE_RSP;
		NetHelper::copy_net_address(&rsp.associate_rsp.parent_address, &network.mac);

//		printf("from address:");
//		NetHelper::printf_address(&rsp.associate_rsp.parent_address);
//		printf("to address:");
//		NetHelper::printf_address(&msg->from_addr);

		nw->sendto(&msg->from_addr, &rsp);
	}

	void handle_network_assignment_req(struct network_assignment_req *msg){
//		printf("%s\n", __FUNCTION__);
		// We need to respond with a new address, subnet.
		int ret;
		union mesh_internal_msg rsp;
		rsp.header.hop_count = 0;
		rsp.header.msgno = MSGNO::NETWORK_ASSIGNMENT_RSP;

		ret = getSubnetToChild(&rsp.assignment_rsp.new_address);
		if(ret) {
			/* could not get a new subnet.
			 * this needs to be sorted, if this will cause
			 * a child not to join the network.
			 */
			return;
		}
		NetHelper::copy_net_address(&rsp.assignment_rsp.parent, &network.mac);

//		printf("from address:");
//		NetHelper::printf_address(&rsp.assignment_rsp.parent);
//		printf("to address:");
//		NetHelper::printf_address(&msg->from_address);

		nw->sendto(&msg->from_address, &rsp);

	}

	void handle_network_assignment_rsp(union mesh_internal_msg *msg){
//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
		NetHelper::queue_add(&network, msg);
	}

	void handle_register_to_master_req(struct register_to_master_req *msg){
//		printf("%s\n", __FUNCTION__);
		union mesh_internal_msg rsp;
		const struct net_address *dst;

		// Drop packet. not valid anymore.
		msg->header.hop_count++;
		if(msg->header.hop_count >= MAX_HOPS) return;

		// I'm not master, pass it along
		// Here we should do routing algorithm
		if(!network.mac.master){
			dst = NetHelper::getRouteAddress(&network, &MASTER);
			nw->sendto(&network.parent, (mesh_internal_msg*)msg);
			return;
		}

		/* I think we need to talk to an external application,
		 * and then respond back?
		 * For now, we respond back, saying OK!
		 */
		// Print out the req:
//		printf("Registrered address: ");
//		NetHelper::printf_address(&msg->host_addr);

		rsp.header.hop_count = 0;
		rsp.reg_master_rsp.header.msgno = MSGNO::REGISTER_TO_MASTER_RSP;
		rsp.reg_master_rsp.status = STATUS::OK;
		NetHelper::copy_net_address(&rsp.reg_master_rsp.destination,
		                            &msg->host_addr);
//		printf("%s: %s\n", getName(),__FUNCTION__);


		dst = NetHelper::getRouteAddress(&network,
		                                  &rsp.reg_master_rsp.destination);

//		printf("route address:");
//		NetHelper::printf_address(dst);
//
//		printf("%s: master sending to: ", getName());
//		NetHelper::printf_address(&rsp.reg_master_rsp.destination);
		nw->sendto(dst, &rsp);
//		printf("%s: %s\n", getName(),__FUNCTION__);

	}

	void handle_register_to_master_rsp(union mesh_internal_msg *msg){
//		printf("%s\n", __FUNCTION__);
		const struct net_address *dst;

		// Drop packet. not valid anymore.
		msg->header.hop_count++;
		if(msg->header.hop_count >= MAX_HOPS) return;

		// I'm not master, pass it along
		// Here we should do routing algorithm
		if(!NetHelper::compare_net_address(&network.mac,
		                                   &msg->reg_master_rsp.destination)){
			dst = NetHelper::getRouteAddress(
					&network, &msg->reg_master_rsp.destination);
//			printf("%s: route address:", getName());
//			NetHelper::printf_address(dst);
			nw->sendto(dst, (mesh_internal_msg*)msg);
			return;
		}

		NetHelper::queue_add(&network, msg);

		// I don't know, set something to someone, to do a register.
		// Perhaps pass it to another application on host.

//		NetHelper::queue_add(&network, msg);
	}

	void network_recv(union mesh_internal_msg *msg) override {
#if DEBUG
//		std::cout << "node name: " << this->name << std::endl;
#endif
		switch(msg->header.msgno) {
		case MSGNO::BROADCAST_ASSOCIATE_REQ:
//			printf("%s: BROADCAST_ASSOCIATE_REQ\n", this->name);
//			printf("mPaired: %d\n", network.mPaired);
//			printf("network.pairedChildren: %d\n", network.pairedChildren);
			if(true == network.mPaired && network.pairedChildren < NEIGHBOURS_SZ) {
				handle_associate_req(&msg->associate_req);
				return;
			}
			break;
		case MSGNO::BROADCAST_ASSOCIATE_RSP:
//			printf("%s: BROADCAST_ASSOCIATE_RSP\n", this->name);
			handle_associate_rsp(msg);
			break;
		case MSGNO::NETWORK_ASSIGNMENT_REQ:
//			printf("%s: NETWORK_ASSIGNMENT_REQ\n", this->name);
			handle_network_assignment_req(&msg->assignment_req);
			break;
		case MSGNO::NETWORK_ASSIGNMENT_RSP:
//			printf("%s: NETWORK_ASSIGNMENT_RSP\n", this->name);
			handle_network_assignment_rsp(msg);
			break;
		case MSGNO::REGISTER_TO_MASTER_REQ:
//			printf("%s: REGISTER_TO_MASTER_REQ\n", this->name);
			handle_register_to_master_req(&msg->reg_master_req);
			break;
		case MSGNO::REGISTER_TO_MASTER_RSP:
//			printf("%s: REGISTER_TO_MASTER_RSP\n", this->name);
			handle_register_to_master_rsp(msg);
			break;
		case MSGNO::BROADCAST_NEIGHBOUR_REQ:
		case MSGNO::BROADCAST_NEIGHBOUR_RSP:
		case MSGNO::MESSAGE_REQ:
		case MSGNO::MESSAGE_RSP:
		case MSGNO::INVALID:
//			printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
			break;
		}
	}

	void setPaired(bool val){network.mPaired = val;}
	bool getPaired(){ return network.mPaired;}
	bool getRegisteredToMaster() { return network.registeredToMaster;}
	char *getName() { return name;}


private:

	void doAssociateReq(){
//		printf("%s\n", __FUNCTION__);
//		printf("NAME: %s\n", name);
		union mesh_internal_msg msg;
		msg.associate_req.header.msgno = MSGNO::BROADCAST_ASSOCIATE_REQ;
		NetHelper::copy_net_address(&msg.associate_req.from_addr, &network.mac);
		nw->sendto(&BROADCAST, &msg);
	}

	void doRegisterReq(){
//		printf("%s\n", __FUNCTION__);
		union mesh_internal_msg msg;
		msg.associate_req.header.msgno = MSGNO::REGISTER_TO_MASTER_REQ;
		NetHelper::copy_net_address(&msg.reg_master_req.host_addr, &network.mac);
		nw->sendto(&network.parent, &msg);
	}

	void publishMeshMessage() {
		MeshMessagePublisher::sub->msg_recv();
	}

	void ping_parent() {
		/* This is our default route to the network
		 * If this fails too many times, we have to disband and
		 * try to form a new network
		 */


	}
	void ping_neighbours() {
		// this shall happen occasionally.
	}

	/**
	 * a normal member taking two arguments and returning an integer value.
	 * @param a an integer argument.
	 * @param s a constant character pointer.
	 * @see Javadoc_Test()
	 * @see ~Javadoc_Test()
	 * @see testMeToo()
	 * @see publicVar()
	 * @return The test results
	 */
	int testMe(int a,const char *s);

	/**
	 * Update the network infrastructure, pinging the parent and neighbours.
	 * @return void
	 */
};

} /* namespace mesh */

#endif /* SRC_MESH_H_ */
