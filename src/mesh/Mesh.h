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

/* // */
#include "NetworkInterfaceSubscriber.h"
#include "MeshMessagePublisher.h"
#include <stdint.h>
#include "Constants.h"
#include <thread>         // std::thread

namespace mesh {
class NetworkData;
}

namespace network {
class NetworkInterface;
}

namespace NetAlgorithm {
class NetAlgorithmInterface;
}

union mesh_internal_msg;
struct networkData;
struct net_address;

namespace mesh {
struct stateData;
static constexpr int MAX_NAME(50);
static constexpr int CHILD_COUNT(5);

using namespace network;
class Mesh : public MeshMessagePublisher, NetworkInterfaceSubscriber {
private:
	NetworkData *network;  /*!< Detailed description after the member */
	NetworkInterface *nw;
	NetAlgorithm::NetAlgorithmInterface *algorithm;
	char name[MAX_NAME];
	uint8_t childs[CHILD_COUNT];
	bool mSetMaster;

	/* State machine */
	struct stateData *statedata;

	/* Timer */
	/* in linux this will not be a callback function
	 * this is something to simulate the microcontroller
	*/
	bool timerStarted;
	bool timerDone;
	std::thread *mThread;

public:
	Mesh(NetworkInterface *nw);
	virtual ~Mesh();
	void setMaster();
	int run();
	void setName(char *name);
	void getParent(struct net_address *addr);
	bool setTemporaryMacAddr(const struct net_address *mac);
	bool getPaired();
	bool getRegisteredToMaster();
	char *getName();

private:
	/* Timer */
	void timerCallback(int ms);
	void armTimer(int ms);

	/* State Machine */
	void initStateMachine();
	void stateMachine();
	void sm_init();

	void sm_master();

	void sm_broadcast_associate_req(union mesh_internal_msg *msg);
	void sm_broadcast_associate_rsp(union mesh_internal_msg *msg);
	void sm_network_assignment_req(union mesh_internal_msg *msg);
	void sm_register_to_master_req(union mesh_internal_msg *msg);
	void sm_ping_parent_req(union mesh_internal_msg *msg);
	void sm_ping_parent_rsp(union mesh_internal_msg *msg);


	void sm_starting_seeking_parent();
	void sm_starting_choosing_parent();
	void sm_starting_register_to_parent();
	void sm_starting_waiting_for_parent();
	void sm_starting_register_to_master();
	void sm_starting_waiting_for_master();
	void sm_starting(); /* Main starting */

	void act_on_messages();

	void sm_started_idle();
	void sm_started_seeking_parent();
	void sm_started_choosing_parent();
	void sm_started_seeking_neighbours();
	void sm_started_choosing_neighbours();
	void sm_started_ping_parent();
	void sm_started_ping_waiting_for_parent();
	void sm_started_ping_neighbours();
	void sm_started_ping_waiting_for_neighbours();
	void sm_started_check_children_keepalive_timers();
	void sm_started(); /* Main started */

	void sm_stopping() {}
	void sm_stopped() {}
	void sm_error() {}

	/* Network related */
	int getSubnetToChild(struct net_address *address);

	void network_recv(union mesh_internal_msg *msg) override;
	void handle_associate_rsp(union mesh_internal_msg *msg);
	void handle_associate_req(union mesh_internal_msg *msg);
	void handle_network_assignment_req(union mesh_internal_msg *msg);
	void handle_network_assignment_rsp(union mesh_internal_msg *msg);
	void handle_register_to_master_req(union mesh_internal_msg *msg);
	void handle_register_to_master_rsp(union mesh_internal_msg *msg);
	void handle_ping_parent_req(union mesh_internal_msg *msg);
	void handle_ping_parent_rsp(union mesh_internal_msg *msg);
	void handle_disconnect_req(union mesh_internal_msg *msg);

	void setPaired(bool val);

	/* */
	int timer_counter_ping_nb;
	int timer_counter_ping_parent;
	int timer_counter_bc_nb;
	int timer_counter_bc_parent;
	int keepalive_parent;
	int keepalive_nb[CHILDREN_SZ];

	void decrease_timer_counters();
	void decrease_parent_timer();
	void decrease_nbs_timer();
	void init_timer_counters();
	void clear_timer_counters();

	/* Mesh related */
	void doAssociateReq();
	void doRegisterReq();
	void doPingParentReq();
};

} /* namespace mesh */

