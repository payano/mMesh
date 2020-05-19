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

namespace debugger {
class DebugInterface;
class DebugSingleton;
}

namespace syscalls {
class SyscallsInterface;
}

namespace mesh {
class NetworkData;
class MeshNetworkHandler;
}

namespace network {
class NetworkInterface;
}

namespace NetAlgorithm {
class NetAlgorithmInterface;
}

union mesh_internal_msg;
struct net_address;

namespace mesh {
struct stateData;
static constexpr int MAX_NAME(50);
static constexpr int CHILD_COUNT(5);

using namespace network;
class Mesh : public MeshMessagePublisher {
private:
	NetworkData *network;  /*!< Detailed description after the member */
	NetworkInterface *nw;
	NetAlgorithm::NetAlgorithmInterface *algorithm;
	MeshNetworkHandler *networkHandler;
	syscalls::SyscallsInterface *syscall;
	char name[MAX_NAME];
	uint8_t childs[CHILD_COUNT];
	bool mSetMaster;
	debugger::DebugInterface *debugger;

	/* State machine */
	struct stateData *statedata;

	/* Timer */
	/* in linux this will not be a callback function
	 * this is something to simulate the microcontroller
	*/
//	bool timerStarted;
//	bool timerDone;
//	std::thread *mThread;

public:
	Mesh(NetworkInterface *nw, syscalls::SyscallsInterface *syscalls);
	virtual ~Mesh();
	void setMaster();
	int run();
	void setName(char *name);
	void getParent(struct net_address *addr);
	bool setTemporaryMacAddr(const struct net_address *mac);
	bool getPaired();
	bool getRegisteredToMaster();
	int getNeighbourCount();
	char *getName();
	void set_generated_mac_addr();
private:
	/* Timer */
	void timerCallback(int ms);
	void armTimer(int ms);

	/* State Machine */
	void initStateMachine();
	void stateMachine();
	void sm_init();
	void sm_error() {}

	void sm_master();

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

	void change_started_state();
	void sm_started(); /* Main started */

	void check_children_keepalive_timers();
	void check_neighbour_keepalive_timers();

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
	void decrease_childs_timer();
	void decrease_nb_timer();
	void init_timer_counters();
	void clear_timer_counters();

};

} /* namespace mesh */

