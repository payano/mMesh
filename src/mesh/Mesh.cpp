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

#include "Mesh.h"
#include "NetworkInterface.h"
#include "NetworkInterfacePublisher.h"
#include "DataTypes.h"


/* should be removed later */
//#include <stdio.h>
//#include <iostream>
#include <unistd.h>

namespace mesh {

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


Mesh::Mesh(NetworkInterface *nw) : nw(nw) {
	network = new networkData;
	NetHelper::init_networkData(network);
	nw->registerSubscriber(this);
	statedata = new stateData;
	initStateMachine();
	timerStarted = false;
	timerDone = false;
	mSetMaster = false;
	for(int i = 0 ; i < MAX_NAME; ++i) {name[i] = 0;}
	for(int i = 0 ; i < CHILD_COUNT; ++i){childs[i] = 0x0;}
}
Mesh::~Mesh() {
	delete nw;
	delete statedata;
}

void Mesh::setMaster() {
	char name[] = "master";
	setPaired(true);
	mSetMaster = true;
	setName(name);
	setTemporaryMacAddr(&MASTER);
}

int Mesh::run() {
	stateMachine();
	return 0;
}

void Mesh::setName(char *name)
{
	char *thisName = this->name;
	while(*name != '\0') {
		*thisName = *name;
		thisName++;
		name++;
	}
}

void Mesh::getParent(struct net_address *addr){
	NetHelper::copy_net_address(addr, &network->parent);
}

bool Mesh::setTemporaryMacAddr(const struct net_address *mac){
	NetHelper::copy_net_address(&network->mac, mac);
	nw->setAddr(mac);
	return true;
}

bool Mesh::getPaired(){ return network->mPaired;}
bool Mesh::getRegisteredToMaster() { return network->registeredToMaster;}
char *Mesh::getName() { return name;}

void Mesh::timerCallback(int ms)
{
	timerStarted = true;
	usleep(ms*1000);
	timerDone = true;
	timerStarted = false;
}


void Mesh::armTimer(int ms)
{
	/* Only one at the time */
	if(timerStarted) return;

	/* this should access a hw timer */
	timerDone = false;
	mThread = new std::thread(&Mesh::timerCallback, this, ms);
}

void Mesh::initStateMachine(){statedata->topState = STATE::STATE_INIT;}

void Mesh::stateMachine()
{
	switch(statedata->topState) {
	case STATE_INIT:
		sm_init();
		break;
	case STATE_MASTER:
		break;
	case STATE_STARTING:
		sm_starting();
		break;
	case STATE_STARTED:
		sm_started();
		break;
	case STATE_STOPPING:
		sm_stopping();
		break;
	case STATE_STOPPED:
		sm_stopped();
		break;
	case STATE_NONE:
	case STATE_ERROR:
		sm_error();
		break;
	}
}

void Mesh::sm_init()
{
	// setup network hardware, ...
	// Generate address

	// when setup is done.
	if(mSetMaster) {
		statedata->topState = STATE::STATE_MASTER;
		return;
	}

	statedata->topState = STATE::STATE_STARTING;
	statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
}

void Mesh::sm_starting_seeking_parent()
{
	armTimer(100); /* 100 ms */
	statedata->starting_state = STARTING_STATE::STARTING_CHOOSING_PARENT;
	doAssociateReq();
}

void Mesh::sm_starting_choosing_parent() {
	/* Here we also get incoming broadcast messages */
	// Check incoming msg, sorting might be good.
	if(!timerDone) return;

	if(0 == NetHelper::queue_sz(network)) {
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}
	// Choose the right one to associate with.
	// We take the first one and discard the rest.
	union mesh_internal_msg *queue_msg;
	NetHelper::queue_get(network, &queue_msg);

	if(MSGNO::BROADCAST_ASSOCIATE_RSP != queue_msg->header.msgno){
		// This is error
		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		delete queue_msg;
		return;
	}

	NetHelper::queue_clear(network);
	struct broadcast_associate_rsp *queue_rsp = &queue_msg->associate_rsp;

	union mesh_internal_msg rsp;
	rsp.assignment_req.header.msgno = MSGNO::NETWORK_ASSIGNMENT_REQ;
	NetHelper::copy_net_address(&rsp.associate_req.from_addr, &network->mac);
	nw->sendto(&queue_rsp->parent_address, &rsp);
	delete queue_msg;

	// Done
	statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_PARENT;
}


void Mesh::sm_starting_register_to_parent()
{
	armTimer(100); /* 100 ms */
	statedata->starting_state = STARTING_STATE::STARTING_WAITING_FOR_PARENT;
}

void Mesh::sm_starting_waiting_for_parent()
{
	if(!timerDone) return;

	if(0 == NetHelper::queue_sz(network))
	{
		// No answer, restart.
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}

	union mesh_internal_msg *queue_msg;
	NetHelper::queue_get(network, &queue_msg);
	NetHelper::queue_clear(network);
	if(MSGNO::NETWORK_ASSIGNMENT_RSP != queue_msg->header.msgno){
		// Error
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		delete queue_msg;
		return;
	}

	struct network_assignment_rsp *msg = &queue_msg->assignment_rsp;
	NetHelper::copy_net_address(&network->parent, &msg->parent);
	NetHelper::copy_net_address(&network->mac, &msg->new_address);
	nw->setAddr(&network->mac);
	network->mPaired = true;

	delete queue_msg;
	// Change state
	statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
}

void Mesh::sm_starting_register_to_master()
{
	armTimer(100); /* 100 ms */
	doRegisterReq();
	statedata->starting_state = STARTING_STATE::STARTING_WAITING_FOR_MASTER;
}

void Mesh::sm_starting_waiting_for_master()
{
	if(!timerDone) return;

	if(0 == NetHelper::queue_sz(network))
	{
		// No answer, restart.
		statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
		return;
	}

	union mesh_internal_msg *queue_msg;
	NetHelper::queue_get(network, &queue_msg);
	NetHelper::queue_clear(network);
	if(MSGNO::REGISTER_TO_MASTER_RSP != queue_msg->header.msgno){
		// Error
		statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
		delete queue_msg;
		return;
	}

	struct register_to_master_rsp *msg = &queue_msg->reg_master_rsp;
	network->registeredToMaster = msg->status == STATUS::OK ? true : false;
	delete queue_msg;

	if(!network->registeredToMaster) {
		statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
		return;
	}

	statedata->topState = STATE::STATE_STARTED;
}

void Mesh::sm_starting()
{
	// try to get online.
	switch(statedata->starting_state) {
	case STARTING_SEEKING_PARENT:
		sm_starting_seeking_parent();
		break;
	case STARTING_CHOOSING_PARENT:
		sm_starting_choosing_parent();
		break;
	case STARTING_REGISTER_TO_PARENT:
		sm_starting_register_to_parent();
		break;
	case STARTING_WAITING_FOR_PARENT:
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

void Mesh::sm_started() {}

int Mesh::getSubnetToChild(struct net_address *address)
{
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

	ret = NetHelper::generate_child_address(network, address);
	if(ret) {
		return -1;
	}

	return 0;
}

void Mesh::network_recv(union mesh_internal_msg *msg) {
	switch(msg->header.msgno) {
	case MSGNO::BROADCAST_ASSOCIATE_REQ:
		if(true == network->mPaired && network->pairedChildren < NEIGHBOURS_SZ) {
			handle_associate_req(msg);
			return;
		}
		break;
	case MSGNO::BROADCAST_ASSOCIATE_RSP:
		handle_associate_rsp(msg);
		break;
	case MSGNO::NETWORK_ASSIGNMENT_REQ:
		handle_network_assignment_req(msg);
		break;
	case MSGNO::NETWORK_ASSIGNMENT_RSP:
		handle_network_assignment_rsp(msg);
		break;
	case MSGNO::REGISTER_TO_MASTER_REQ:
		handle_register_to_master_req(msg);
		break;
	case MSGNO::REGISTER_TO_MASTER_RSP:
		handle_register_to_master_rsp(msg);
		break;
	case MSGNO::BROADCAST_NEIGHBOUR_REQ:
	case MSGNO::BROADCAST_NEIGHBOUR_RSP:
	case MSGNO::MESSAGE_REQ:
	case MSGNO::MESSAGE_RSP:
	case MSGNO::INVALID:
		break;
	}
}

void Mesh::handle_associate_rsp(union mesh_internal_msg *msg) {
	/* Okay, someone responded. There is a network to be
	 * associated with, ask the responder for a address.
	 */
	NetHelper::queue_add(network, msg);
	return;
}

void Mesh::handle_associate_req(union mesh_internal_msg *msg){
	union mesh_internal_msg rsp;
	rsp.header.hop_count = 0;
	rsp.header.msgno = MSGNO::BROADCAST_ASSOCIATE_RSP;
	NetHelper::copy_net_address(&rsp.associate_rsp.parent_address, &network->mac);
	nw->sendto(&msg->associate_req.from_addr, &rsp);
}

void Mesh::handle_network_assignment_req(union mesh_internal_msg *msg){
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

	NetHelper::copy_net_address(&rsp.assignment_rsp.parent, &network->mac);
	nw->sendto(&msg->assignment_req.from_address, &rsp);
}

void Mesh::handle_network_assignment_rsp(union mesh_internal_msg *msg){
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_register_to_master_req(union mesh_internal_msg *msg){
	union mesh_internal_msg rsp;
	const struct net_address *dst;

	// Drop packet. not valid anymore.
	msg->header.hop_count++;
	if(msg->header.hop_count >= MAX_HOPS) return;

	// I'm not master, pass it along
	// Here we should do routing algorithm
	if(!network->mac.master){
		dst = NetHelper::getRouteAddress(network, &MASTER);
		nw->sendto(&network->parent, (mesh_internal_msg*)msg);
		return;
	}

	/* I think we need to talk to an external application,
	 * and then respond back?
	 * For now, we respond back, saying OK!
	 */
	rsp.header.hop_count = 0;
	rsp.reg_master_rsp.header.msgno = MSGNO::REGISTER_TO_MASTER_RSP;
	rsp.reg_master_rsp.status = STATUS::OK;
	NetHelper::copy_net_address(&rsp.reg_master_rsp.destination,
	                            &msg->reg_master_req.host_addr);

	dst = NetHelper::getRouteAddress(network,
	                                  &rsp.reg_master_rsp.destination);

	nw->sendto(dst, &rsp);
}

void Mesh::handle_register_to_master_rsp(union mesh_internal_msg *msg){
	const struct net_address *dst;

	// Drop packet. not valid anymore.
	msg->header.hop_count++;
	if(msg->header.hop_count >= MAX_HOPS) return;

	// I'm not master, pass it along
	// Here we should do routing algorithm
	if(!NetHelper::compare_net_address(&network->mac,
	                                   &msg->reg_master_rsp.destination)){
		dst = NetHelper::getRouteAddress(
				network, &msg->reg_master_rsp.destination);
		nw->sendto(dst, (mesh_internal_msg*)msg);
		return;
	}

	NetHelper::queue_add(network, msg);
}


void Mesh::doAssociateReq(){
	union mesh_internal_msg msg;
	msg.associate_req.header.msgno = MSGNO::BROADCAST_ASSOCIATE_REQ;
	NetHelper::copy_net_address(&msg.associate_req.from_addr, &network->mac);
	nw->sendto(&BROADCAST, &msg);
}

void Mesh::doRegisterReq(){
	union mesh_internal_msg msg;
	msg.associate_req.header.msgno = MSGNO::REGISTER_TO_MASTER_REQ;
	NetHelper::copy_net_address(&msg.reg_master_req.host_addr, &network->mac);
	nw->sendto(&network->parent, &msg);
}

void Mesh::setPaired(bool val){network->mPaired = val;}


} /* namespace mesh */
