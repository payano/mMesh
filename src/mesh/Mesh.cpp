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
#include "LazyAlgorithm.h"
/* should be removed later */
//#include <stdio.h>
#include <unistd.h>
#include <chrono>
//#include <iostream>

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
	STARTED_SEEKING_PARENT,
	STARTED_CHOOSING_PARENT,
	STARTED_SEEKING_NEIGHBOURS,
	STARTED_CHOOSING_NEIGHBOURS,
	STARTED_PING_PARENT,
	STARTED_PING_WAITING_FOR_PARENT,
	STARTED_PING_NEIGHBOURS,
	STARTED_PING_WAITING_FOR_NEIGHBOURS,
	STARTED_CHECK_CHILDREN_KEEPALIVE_TIMERS,
};

struct stateData {
	enum STATE topState;
	enum STARTING_STATE starting_state;
	enum STARTED_STATE started_state;
};

Mesh::Mesh(NetworkInterface *nw) : nw(nw) {
	network = new networkData;
	NetHelper::init_networkData(network);
	nw->registerSubscriber(this);
	statedata = new stateData;
	algorithm = new NetAlgorithm::LazyAlgorithm();

	initStateMachine();
	timerStarted = false;
	timerDone = false;
	mSetMaster = false;

	clear_timer_counters();

	mThread = nullptr;
	for(int i = 0 ; i < MAX_NAME; ++i) {name[i] = 0;}
	for(int i = 0 ; i < CHILD_COUNT; ++i){childs[i] = 0x0;}
}
Mesh::~Mesh() {
	delete network;
	delete algorithm;
	if(nullptr != statedata) delete statedata;
	if(nullptr != mThread) {
		mThread->join();
		delete mThread;
	}
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
	char *thisName = &this->name[0];
	char *lastThisName = &(this->name[MAX_NAME-1]);
	while(*name != '\0' && thisName != lastThisName) {
		*thisName = *name;
		thisName++;
		name++;
	}
	*lastThisName = '\0';
}

void Mesh::getParent(struct net_address *addr){
	NetHelper::copy_net_address(addr, &network->parent.mac);
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
	timerDone = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	timerDone = true;
	timerStarted = false;
}

void Mesh::armTimer(int ms)
{
	/* Only one at the time */
	if(timerStarted) return;
	if(nullptr != mThread) {
		mThread->join();
		delete mThread;
		mThread = nullptr;
	}
	/* this should access a hw timer */
	timerStarted = true;
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
		sm_master();
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

void Mesh::sm_broadcast_associate_req(union mesh_internal_msg *msg){
	union mesh_internal_msg rsp;
	rsp.header.hop_count = 0;
	rsp.header.msgno = MSGNO::BROADCAST_ASSOCIATE_RSP;
	NetHelper::copy_net_address(&rsp.associate_rsp.parent_address, &network->mac);
	nw->sendto(&msg->associate_req.from_addr, &rsp);
}

void Mesh::sm_broadcast_associate_rsp(union mesh_internal_msg *msg)
{
	(void)msg;
}

void Mesh::sm_network_assignment_req(union mesh_internal_msg *msg)
{
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
	printf("SENDING TO: ");
	NetHelper::printf_address(&msg->assignment_req.from_address);
}

void Mesh::sm_register_to_master_req(union mesh_internal_msg *msg)
{
	union mesh_internal_msg rsp;
	const struct net_address *dst;

	/* I think we need to talk to an external application,
	 * and then respond back?
	 * For now, we respond back, saying OK!
	 */
	rsp.header.hop_count = 0;
	rsp.reg_master_rsp.header.msgno = MSGNO::REGISTER_TO_MASTER_RSP;
	rsp.reg_master_rsp.status = STATUS::OK;

	printf("HOST CALLED: ");
	NetHelper::printf_address(&msg->reg_master_req.host_addr);

	NetHelper::copy_net_address(&rsp.reg_master_rsp.destination,
	                            &msg->reg_master_req.host_addr);

	dst = algorithm->getRouteForPacket(network,
	                                   &rsp.reg_master_rsp.destination);
	nw->sendto(dst, &rsp);
	printf("%s -- DST ADDR: ", getName());
	NetHelper::printf_address(dst);
	printf("%s -- TO: ", getName());
	NetHelper::printf_address(&msg->reg_master_rsp.destination);

}

void Mesh::sm_master()
{
	act_on_messages();
	if(timerStarted) return;

	armTimer(100); // 10sec

//	sm_started_check_children_keepalive_timers();
//	decrease_timer_counters();
//	decrease_nbs_timer();
}

void Mesh::sm_init()
{
	// setup network hardware, ...
	// Generate address
	init_timer_counters();

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
	armTimer(300); /* 100 ms */
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

	int ret;
	struct net_address parent;
	ret = algorithm->choose_parent_from_list(network, &parent);

	if(ret) {
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}

	printf("%s: STARTING_CHOOSING_PARENT\n", getName());
	union mesh_internal_msg rsp;
	rsp.header.msgno = MSGNO::NETWORK_ASSIGNMENT_REQ;
	rsp.header.hop_count = 0;
	NetHelper::copy_net_address(&rsp.associate_req.from_addr, &network->mac);

	nw->sendto(&parent, &rsp);

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

	union mesh_internal_msg queue_msg;
	NetHelper::queue_get(network, &queue_msg);
	NetHelper::queue_clear(network);
	if(MSGNO::NETWORK_ASSIGNMENT_RSP != queue_msg.header.msgno){
		// Error
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}

	printf("%s: STARTING_WAITING_FOR_PARENT\n", getName());
	struct network_assignment_rsp *msg = &queue_msg.assignment_rsp;
	NetHelper::copy_net_address(&network->parent.mac, &msg->parent);
	NetHelper::copy_net_address(&network->mac, &msg->new_address);
	nw->setAddr(&network->mac);
	network->mPaired = true;

	// Change state
	statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
}

void Mesh::sm_starting_register_to_master()
{
	armTimer(1000); /* 100 ms */
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

	union mesh_internal_msg queue_msg;
	NetHelper::queue_get(network, &queue_msg);
	NetHelper::queue_clear(network);
	if(MSGNO::REGISTER_TO_MASTER_RSP != queue_msg.header.msgno){
		// Error
		statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
		return;
	}

	struct register_to_master_rsp *msg = &queue_msg.reg_master_rsp;
	network->registeredToMaster = msg->status == STATUS::OK ? true : false;

	printf("%s: Regged: %d\n", getName(), network->registeredToMaster);
	printf("%s: STARTING_WAITING_FOR_MASTER\n", getName());

	if(!network->registeredToMaster) {
		statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
		return;
	}

	statedata->topState = STATE::STATE_STARTED;
	statedata->started_state = STARTED_STATE::STARTED_IDLE;
	init_timer_counters();
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

void Mesh::init_timer_counters()
{
	timer_counter_ping_nb = TIMER_COUNTER_PING_NB;
	timer_counter_ping_parent  = TIMER_COUNTER_PING_PARENT;
	timer_counter_bc_nb = TIMER_COUNTER_BC_NB;
	timer_counter_bc_parent = TIMER_COUNTER_BC_PARENT;
	keepalive_parent = TIMER_KEEPALIVE;
	int *keep_nb = keepalive_nb;
	for(int i = 0 ; i < CHILDREN_SZ; ++i, keep_nb++) {
		*keep_nb = 0;
	}
}

void Mesh::clear_timer_counters()
{
	timer_counter_ping_nb = 0;
	timer_counter_ping_parent  = 0;
	timer_counter_bc_nb = 0;
	timer_counter_bc_parent = 0;
	keepalive_parent = 0;
}

void Mesh::decrease_timer_counters()
{
	if(timer_counter_ping_nb > 0)     --timer_counter_ping_nb;
	if(timer_counter_ping_parent > 0) --timer_counter_ping_parent;
	if(timer_counter_bc_nb > 0)       --timer_counter_bc_nb;
	if(timer_counter_bc_parent > 0)   --timer_counter_bc_parent;
}

void Mesh::decrease_parent_timer()
{
	return;
	NetHelper::decrease_parent_timer(network, TIMER_DECREASE);
}

void Mesh::decrease_nbs_timer()
{
	NetHelper::decrease_neighbour_timers(network, TIMER_DECREASE);
}

void Mesh::act_on_messages()
{
//	printf("%s: queue sz: %d\n", getName(), NetHelper::queue_sz(network));

	union mesh_internal_msg msg;
//	int i = 0;
	while(!NetHelper::queue_get(network, &msg)){
//		printf("%s: int: %d\n", getName(), i++);
		switch(msg.header.msgno) {
		case MSGNO::BROADCAST_ASSOCIATE_REQ:
			printf("%s: BROADCAST_ASSOCIATE_REQ\n", getName());
			sm_broadcast_associate_req(&msg);
			break;
		case MSGNO::BROADCAST_NEIGHBOUR_REQ:
			printf("%s: BROADCAST_NEIGHBOUR_REQ\n", getName());
			break;
		case MSGNO::BROADCAST_NEIGHBOUR_RSP:
			printf("%s: BROADCAST_NEIGHBOUR_RSP\n", getName());
			break;
		case MSGNO::NETWORK_ASSIGNMENT_REQ:
			printf("%s: NETWORK_ASSIGNMENT_REQ\n", getName());
			sm_network_assignment_req(&msg);
			break;
		case MSGNO::REGISTER_TO_MASTER_REQ:
			printf("%s: - REGISTER_TO_MASTER_REQ\n", getName());
			sm_register_to_master_req(&msg);
			break;
		case MSGNO::REGISTER_TO_MASTER_RSP:
			printf("%s: PING_PARENT_REQ\n", getName());
			break;
		case MSGNO::PING_PARENT_REQ:
//			printf("%s: PING_PARENT_REQ\n", getName());
			break;
		case MSGNO::PING_PARENT_RSP:
//			printf("%s: PING_PARENT_RSP\n", getName());
			break;
		case MSGNO::DISCONNECT_CHILD_REQ:
//			printf("%s: DISCONNECT_CHILD_REQ\n", getName());
			break;
		case MSGNO::DISCONNECT_CHILD_RSP:
//			printf("%s: DISCONNECT_CHILD_RSP\n", getName());
			break;
		case MSGNO::MESSAGE_REQ:
//			printf("%s: MESSAGE_REQ\n", getName());
			break;
		case MSGNO::MESSAGE_RSP:
//			printf("%s: MESSAGE_RSP\n", getName());
			break;
			/* Connected nodes will not do a req, therefore not get a response.
			 */
		case MSGNO::BROADCAST_ASSOCIATE_RSP:
		case MSGNO::NETWORK_ASSIGNMENT_RSP:
		case MSGNO::INVALID:
			printf("%s: INVALID\n", getName());
			break;
		}
	}
}

void Mesh::sm_started_idle()
{
	act_on_messages();

	if(timerStarted) return;

	armTimer(100); // 10sec

	/* do more seldom */
	statedata->started_state = STARTED_STATE::STARTED_CHECK_CHILDREN_KEEPALIVE_TIMERS;

	// Prio
	if(0 == timer_counter_ping_parent) {
		statedata->started_state = STARTED_STATE::STARTED_PING_PARENT;
		timer_counter_ping_parent = TIMER_COUNTER_PING_PARENT;
	}else if(0 == timer_counter_ping_nb) {
		statedata->started_state = STARTED_STATE::STARTED_PING_NEIGHBOURS;
		timer_counter_ping_nb = TIMER_COUNTER_PING_NB;
	}else if(0 == timer_counter_bc_parent) {
		statedata->started_state = STARTED_STATE::STARTED_SEEKING_PARENT;
		timer_counter_bc_parent = TIMER_COUNTER_BC_PARENT;
	}else if(0 == timer_counter_bc_nb) {
		statedata->started_state = STARTED_STATE::STARTED_SEEKING_NEIGHBOURS;
		timer_counter_bc_nb = TIMER_COUNTER_BC_NB;
	}
	decrease_timer_counters();
	decrease_parent_timer();
	decrease_nbs_timer();
}

void Mesh::sm_started_seeking_parent()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	statedata->started_state = STARTED_STATE::STARTED_CHOOSING_PARENT;
}

void Mesh::sm_started_choosing_parent()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	statedata->started_state = STARTED_STATE::STARTED_IDLE;
}

void Mesh::sm_started_seeking_neighbours()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	statedata->started_state = STARTED_STATE::STARTED_CHOOSING_NEIGHBOURS;
}

void Mesh::sm_started_choosing_neighbours()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	statedata->started_state = STARTED_STATE::STARTED_IDLE;
}

void Mesh::sm_started_ping_parent()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	armTimer(100); /* 100 ms */
	doPingParentReq();
	statedata->started_state = STARTED_STATE::STARTED_PING_WAITING_FOR_PARENT;

}

void Mesh::sm_started_ping_waiting_for_parent()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	if(!timerDone) return;

	statedata->started_state = STARTED_STATE::STARTED_IDLE;
	if(0 == NetHelper::queue_sz(network))
	{
		// No answer, dont update the counter.
		return;
	}

	// Only check first, we only have one parent.
	union mesh_internal_msg queue_msg;
	NetHelper::queue_get(network, &queue_msg);
	NetHelper::queue_clear(network);
	if(MSGNO::PING_PARENT_RSP != queue_msg.header.msgno){
		// Error
		return;
	}

//	printf("%s: PARENT RESPONSE!\n", getName());

}

void Mesh::sm_started_ping_neighbours()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	statedata->started_state = STARTED_STATE::STARTED_PING_WAITING_FOR_NEIGHBOURS;
}

void Mesh::sm_started_ping_waiting_for_neighbours()
{
//	printf("%s: %s\n", getName(), __FUNCTION__);
	statedata->started_state = STARTED_STATE::STARTED_IDLE;
}

void Mesh::sm_started_check_children_keepalive_timers()
{
	struct node_data *current = nullptr;
	while(NetHelper::iterateChilds(network, &current)){
		if(!NetHelper::checkConnected(current)) continue;
		if(!NetHelper::check_timer_zero(current)) continue;

		/* We want to disconnect the node.
		 * because we don't consider it to be online any more.
		 */
		struct net_address remove_node;
		int ret = NetHelper::remove_child_node(network, current, &remove_node);
		if(ret) continue;

		union mesh_internal_msg msg;
		msg.header.hop_count = 0;
		// We dont want a respond for this message.
		msg.header.msgno = MSGNO::DISCONNECT_CHILD_REQ;
		NetHelper::copy_net_address(&msg.disconnect_child_req.from, &network->mac);
		NetHelper::copy_net_address(&msg.disconnect_child_req.to, &remove_node);
		nw->sendto(&remove_node, &msg);

	}
	statedata->started_state = STARTED_STATE::STARTED_IDLE;
}
void Mesh::sm_started() {
	switch(statedata->started_state){
	case STARTED_IDLE:
		sm_started_idle();
		break;
	case STARTED_SEEKING_NEIGHBOURS:
		sm_started_seeking_neighbours();
		break;
	case STARTED_CHOOSING_NEIGHBOURS:
		sm_started_choosing_neighbours();
		break;
	case STARTED_PING_PARENT:
		sm_started_ping_parent();
		break;
	case STARTED_PING_WAITING_FOR_PARENT:
		sm_started_ping_waiting_for_parent();
		break;
	case STARTED_PING_NEIGHBOURS:
		sm_started_ping_neighbours();
		break;
	case STARTED_PING_WAITING_FOR_NEIGHBOURS:
		sm_started_ping_waiting_for_neighbours();
		break;
	case STARTED_SEEKING_PARENT:
		sm_started_seeking_parent();
		break;
	case STARTED_CHOOSING_PARENT:
		sm_started_choosing_parent();
		break;
	case STARTED_CHECK_CHILDREN_KEEPALIVE_TIMERS:
		statedata->started_state = STARTED_STATE::STARTED_IDLE;
//		sm_started_check_children_keepalive_timers();
		break;
	}
}

int Mesh::getSubnetToChild(struct net_address *address)
{
	int ret = 0;
	// init subnet
	NetHelper::clear_net_address(address);

	ret = NetHelper::generate_child_address(network, address);
	return ret;
}

void Mesh::network_recv(union mesh_internal_msg *msg) {
	/* These methods will be run from the interrupt vector
	 * They all they is doing is adding the messages to the message queue
	 * If the message destination is to this node.
	 */

	switch(msg->header.msgno) {
	case MSGNO::BROADCAST_ASSOCIATE_REQ:
		handle_associate_req(msg);
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
	case PING_PARENT_REQ:
//		printf("%s: PING_PARENT_REQ\n", getName());
		handle_ping_parent_req(msg);
		break;
	case PING_PARENT_RSP:
//		printf("%s: PING_PARENT_RSP\n", getName());
		handle_ping_parent_rsp(msg);
		break;
	case MSGNO::DISCONNECT_CHILD_REQ:
//		printf("%s: DISCONNECT_CHILD_REQ\n", getName());
		handle_disconnect_req(msg);
		break;
	case MSGNO::DISCONNECT_CHILD_RSP:
	case MSGNO::BROADCAST_NEIGHBOUR_REQ:
	case MSGNO::BROADCAST_NEIGHBOUR_RSP:
	case MSGNO::MESSAGE_REQ:
	case MSGNO::MESSAGE_RSP:
	case MSGNO::INVALID:
		break;
	}
}

void Mesh::handle_associate_rsp(union mesh_internal_msg *msg) {
	printf("%s: BROADCAST_ASSOCIATE_RSP\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_associate_req(union mesh_internal_msg *msg){
	if(false == network->mPaired || network->pairedChildren >= CHILDREN_SZ) {
		return;
	}
	printf("%s: BROADCAST_ASSOCIATE_REQ\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_network_assignment_req(union mesh_internal_msg *msg){
	printf("%s: NETWORK_ASSIGNMENT_REQ\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_network_assignment_rsp(union mesh_internal_msg *msg){
	printf("%s: NETWORK_ASSIGNMENT_RSP\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_register_to_master_req(union mesh_internal_msg *msg){
	// Drop packet. not valid anymore.
	msg->header.hop_count++;
	if(msg->header.hop_count >= MAX_HOPS) {
		return;
	}

	// I'm not master, pass it along
	// Here we should do routing algorithm
	if(!network->mac.master){
		const struct net_address *dst;
		dst = algorithm->getRouteForPacket(network, &MASTER);
		nw->sendto(dst, msg);
		printf("%s: ROUTING NETWORK_ASSIGNMENT_RSP\n", getName());
		return;
	}

	printf("%s: NETWORK_ASSIGNMENT_RSP\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_register_to_master_rsp(union mesh_internal_msg *msg){

	// Drop packet. not valid anymore.
	msg->header.hop_count++;
	if(msg->header.hop_count >= MAX_HOPS) return;

	// I'm not master, pass it along
	// Here we should do routing algorithm
	if(!NetHelper::compare_net_address(&network->mac,
	                                   &msg->reg_master_rsp.destination)){
		const struct net_address *dst = algorithm->getRouteForPacket(
				network, &msg->reg_master_rsp.destination);
		nw->sendto(dst, msg);

		printf("%s: ROUTING REGISTER_TO_MASTER_RSP\n", getName());
		printf("%s: HEADER DEST: ", getName());
		NetHelper::printf_address(&msg->reg_master_rsp.destination);
		printf("%s: ROUTING DST: ", getName());
		NetHelper::printf_address(dst);
		return;
	}

	printf("%s: REGISTER_TO_MASTER_RSP\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_ping_parent_req(union mesh_internal_msg *msg)
{
	// If it is our child, respond.
	if(!NetHelper::isChildOf(&network->mac, &msg->ping_parent_req.from))
		return;

	NetHelper::queue_add(network, msg);
}

void Mesh::handle_ping_parent_rsp(union mesh_internal_msg *msg)
{
	// Check that is ours
	if(!NetHelper::compare_net_address(&network->mac, &msg->ping_parent_rsp.to))
		return;
	printf("%s: handle_ping_parent_rsp\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::handle_disconnect_req(union mesh_internal_msg *msg)
{

	//Check parent
	if(!NetHelper::compare_net_address(&network->parent.mac,
	                               &msg->disconnect_child_req.from)) return;

	printf("%s: handle_ping_parent_rsp\n", getName());
	NetHelper::queue_add(network, msg);
}

void Mesh::doAssociateReq(){
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::BROADCAST_ASSOCIATE_REQ;
	msg.header.hop_count = 0;
	NetHelper::copy_net_address(&msg.associate_req.from_addr, &network->mac);
	nw->sendto(&BROADCAST, &msg);
}

void Mesh::doRegisterReq(){
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::REGISTER_TO_MASTER_REQ;
	msg.header.hop_count = 0;
	printf("%s: doing doRegisterReq: ", getName());
	NetHelper::printf_address(&network->mac);
	NetHelper::copy_net_address(&msg.reg_master_req.host_addr, &network->mac);
	nw->sendto(&network->parent.mac, &msg);
}

void Mesh::doPingParentReq()
{
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::PING_PARENT_REQ;
	msg.header.hop_count = 0;
	NetHelper::copy_net_address(&msg.ping_parent_req.from, &network->mac);
	nw->sendto(&network->parent.mac, &msg);
}
void Mesh::setPaired(bool val){network->mPaired = val;}

} /* namespace mesh */
