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
#include "NetworkData.h"
#include "MeshNetworkHandler.h"
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
	STARTED_SEEKING_NEIGHBOURS,
	STARTED_PING_PARENT,
	STARTED_PING_NEIGHBOURS,
};

struct stateData {
	enum STATE topState;
	enum STARTING_STATE starting_state;
	enum STARTED_STATE started_state;
};

Mesh::Mesh(NetworkInterface *nw) : nw(nw) {
	network = new NetworkData;
	statedata = new stateData;
	algorithm = new NetAlgorithm::LazyAlgorithm();

	networkHandler = new MeshNetworkHandler(this, network, nw, algorithm);

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
	copy_data(addr, &network->parent.mac, sizeof(*addr));
}

bool Mesh::setTemporaryMacAddr(const struct net_address *mac){
	copy_data(&network->mac, mac, sizeof(*mac));
	nw->setAddr(mac);
	return true;
}

bool Mesh::getPaired(){ return network->mPaired;}
void Mesh::setPaired(bool val){network->mPaired = val;}
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
	case STATE_NONE:
	case STATE_ERROR:
		sm_error();
		break;
	}
}

void Mesh::sm_master()
{
	act_on_messages();
	if(timerStarted) return;

	armTimer(100); // 10sec

	decrease_timer_counters();
	decrease_nbs_timer();
	check_children_keepalive_timers();
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
	armTimer(200); /* 100 ms */
	statedata->starting_state = STARTING_STATE::STARTING_CHOOSING_PARENT;
	networkHandler->doBroadcastAssociateReq();
}

void Mesh::sm_starting_choosing_parent() {
	/* Here we also get incoming broadcast messages */
	// Check incoming msg, sorting might be good.
	if(!timerDone) return;

	if(0 == network->queue_sz()) {
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}

	int ret = networkHandler->doChooseParent();
	if(ret) {
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}

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

	if(0 == network->queue_sz())
	{
		// No answer, restart.
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}

	union mesh_internal_msg queue_msg;
	network->queue_get(&queue_msg);
	network->queue_clear();
	if(MSGNO::NETWORK_ASSIGNMENT_RSP != queue_msg.header.msgno){
		// Error
		statedata->starting_state = STARTING_STATE::STARTING_SEEKING_PARENT;
		return;
	}

	networkHandler->doRegisterToMasterReq(&queue_msg);

	// Change state
	statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
}

void Mesh::sm_starting_register_to_master()
{
	armTimer(400); /* 100 ms */
	networkHandler->doRegisterReq();
	statedata->starting_state = STARTING_STATE::STARTING_WAITING_FOR_MASTER;
}

void Mesh::sm_starting_waiting_for_master()
{
	if(!timerDone) return;

	if(0 == network->queue_sz())
	{
		// No answer, restart.
		statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
		return;
	}

	union mesh_internal_msg queue_msg;
	network->queue_get(&queue_msg);
	network->queue_clear();
	if(MSGNO::REGISTER_TO_MASTER_RSP != queue_msg.header.msgno){
		// Error
		statedata->starting_state = STARTING_STATE::STARTING_REGISTER_TO_MASTER;
		return;
	}

	struct register_to_master_rsp *msg = &queue_msg.reg_master_rsp;
	network->registeredToMaster = msg->status == STATUS::OK ? true : false;

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
	network->decrease_parent_timer(TIMER_DECREASE);
}

void Mesh::decrease_nbs_timer()
{
	network->decrease_child_timers(TIMER_DECREASE);
}

void Mesh::act_on_messages()
{
//	printf("%s: queue sz: %d\n", getName(), NetHelper::queue_sz(network));

	union mesh_internal_msg msg;
//	int i = 0;
	while(!network->queue_get(&msg)){
		switch(msg.header.msgno) {
		case MSGNO::BROADCAST_ASSOCIATE_REQ:
//			printf("%s: BROADCAST_ASSOCIATE_REQ\n", getName());
			networkHandler->doBroadcastAssociateRsp(&msg);
			break;
		case MSGNO::BROADCAST_NEIGHBOUR_REQ:
//			printf("%s: BROADCAST_NEIGHBOUR_REQ\n", getName());
			break;
		case MSGNO::NETWORK_ASSIGNMENT_REQ:
//			printf("%s: NETWORK_ASSIGNMENT_REQ\n", getName());
			networkHandler->doNetworkAssignmentRsp(&msg);
			break;
		case MSGNO::REGISTER_TO_MASTER_REQ:
//			printf("%s: REGISTER_TO_MASTER_REQ\n", getName());
			/* I think we need to talk to an external application,
			 * and then respond back?
			 * For now, we respond back, saying OK!
			 */
			networkHandler->doRegisterToMasterRsp(&msg);
			break;
		case MSGNO::PING_PARENT_REQ:
//			printf("%s: PING_PARENT_REQ\n", getName());
			network->updateChildCounter(&msg.ping_parent_req.from);
			networkHandler->doPingParentRsp(&msg);
			break;
		case MSGNO::PING_PARENT_RSP:
//			printf("%s: PING_PARENT_RSP\n", getName());
			network->updateParentCounter(&msg.ping_parent_rsp.from);
			break;
		case MSGNO::DISCONNECT_CHILD_REQ:
			/* Implementation mising */
//			printf("%s: DISCONNECT_CHILD_REQ\n", getName());
			break;
		case MSGNO::DISCONNECT_CHILD_RSP:
			/* Implementation mising */
//			printf("%s: DISCONNECT_CHILD_RSP\n", getName());
			break;
		case MSGNO::MESSAGE_REQ:
			/* Implementation mising */
//			printf("%s: MESSAGE_REQ\n", getName());
			break;
		case MSGNO::MESSAGE_RSP:
			/* Implementation mising */
//			printf("%s: MESSAGE_RSP\n", getName());
			break;

			/* Connected nodes will not do a req, therefore not get a response.
			 */
		case MSGNO::PING_NEIGHBOUR_RSP:
//			printf("%s: PING_NEIGHBOUR_RSP\n", getName());
			break;

		case MSGNO::PING_NEIGHBOUR_REQ:
		case MSGNO::BROADCAST_NEIGHBOUR_RSP:
		case MSGNO::BROADCAST_ASSOCIATE_RSP:
		case MSGNO::NETWORK_ASSIGNMENT_RSP:
		case MSGNO::REGISTER_TO_MASTER_RSP:
		case MSGNO::INVALID:
			printf("%s: INVALID\n", getName());
			break;
		}
	}
}

void Mesh::change_started_state()
{
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
}

void Mesh::check_children_keepalive_timers()
{
	struct node_data *current = nullptr;
	while(network->iterateChilds(&current)){
		if(!NetHelper::checkConnected(current)) continue;
		if(!NetHelper::check_timer_zero(current)) continue;
		networkHandler->doDisconnectChildReq(current);
	}
	statedata->started_state = STARTED_STATE::STARTED_IDLE;
}

void Mesh::sm_started() {
	act_on_messages();
	switch(statedata->started_state){
	case STARTED_IDLE:
		break;
	case STARTED_SEEKING_NEIGHBOURS:
//		printf("%s: STARTED_SEEKING_NEIGHBOURS\n", getName());
		networkHandler->doSeekNeighbours();
		statedata->started_state = STARTED_STATE::STARTED_IDLE;
		break;
	case STARTED_PING_PARENT:
		networkHandler->doPingParentReq();
		statedata->started_state = STARTED_STATE::STARTED_IDLE;
		break;
	case STARTED_PING_NEIGHBOURS:
//		printf("%s: STARTED_PING_NEIGHBOURS\n", getName());
		networkHandler->doPingNeighbours();
		statedata->started_state = STARTED_STATE::STARTED_IDLE;
		break;
	case STARTED_SEEKING_PARENT:
		statedata->started_state = STARTED_STATE::STARTED_IDLE;
		break;
	}

	if(timerStarted) return;
	armTimer(100); // 10sec
	decrease_timer_counters();
	decrease_nbs_timer();
	check_children_keepalive_timers();
	change_started_state();
}

} /* namespace mesh */
