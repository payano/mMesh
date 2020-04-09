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
#ifdef UNIX

#pragma once

#include <list>
#include "DataTypes.h"
#include <stdio.h> // REMOVE

namespace node {
class Node;
}
namespace island {

class Island {
/*
 * this class makes grouping of nodes easier. this is a physical domain
 * so to say, the broadcasts and messaging will only working within one island.
 * A parent is a member of atleast two islands, otherwise that parent would be
 * isolated.
 */
	int m_invalid;
	int m_broadcast_associate_req;
	int m_broadcast_associate_rsp;
	int m_broadcast_neighbour_req;
	int m_broadcast_neighbour_rsp;
	int m_network_assignment_req;
	int m_network_assignment_rsp;
	int m_register_to_master_req;
	int m_register_to_master_rsp;
	int m_message_req;
	int m_message_rsp;
	int m_ping_parent_req;
	int m_ping_parent_rsp;
	int m_disconnect_child_req;
	int m_disconnect_child_rsp;
	int m_ping_neighbour_req;
	int m_ping_neighbour_rsp;


public:
	Island();
	virtual ~Island();
	std::list<node::Node*> members;
	void registerToIsland(node::Node *node);
	void unregisterToIsland(node::Node *node);
	void sendMessage(const struct net_address *to_addr, union mesh_internal_msg *msg);
	void reset();
	void printStatistics() {
		// dont print invalid.
		printf("  bc_ass_req          : %03d, ", m_broadcast_associate_req);
		printf("bc_ass_rsp          : %03d\n", m_broadcast_associate_rsp);
		printf("  bc_nb_req           : %03d, ", m_broadcast_neighbour_req);
		printf("bc_nb_rsp           : %03d\n", m_broadcast_neighbour_rsp);
		printf("  nw_ass_req          : %03d, ", m_network_assignment_req);
		printf("nw_ass_rsp          : %03d\n", m_network_assignment_rsp);
		printf("  reg2master_req      : %03d, ", m_register_to_master_req);
		printf("reg2master_rsp      : %03d\n", m_register_to_master_rsp);
		printf("  ping_parent_req     : %03d, ", m_register_to_master_req);
		printf("ping_parent_rsp     : %03d\n", m_register_to_master_rsp);
		printf("  disconnect_child_req: %03d, ", m_disconnect_child_req);
		printf("disconnect_child_rsp: %03d\n", m_disconnect_child_rsp);
		printf("  msg_req             : %03d, ", m_message_req);
		printf("msg_rsp             : %03d\n", m_message_rsp);
		printf("  m_ping_neighbour_req: %03d, ", m_ping_neighbour_req);
		printf("m_ping_neighbour_rsp: %03d\n", m_ping_neighbour_rsp);
	}
};


} /* namespace node */
#endif
