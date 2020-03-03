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

#include <list>
#include "Islands.h"
#include "Node.h"
#include "Mesh.h"
namespace island {

Island::Island() :
		m_invalid(0),
		m_broadcast_associate_req(0),
		m_broadcast_associate_rsp(0),
		m_broadcast_neighbour_req(0),
		m_broadcast_neighbour_rsp(0),
		m_network_assignment_req(0),
		m_network_assignment_rsp(0),
		m_register_to_master_req(0),
		m_register_to_master_rsp(0),
		m_message_req(0),
		m_message_rsp(0)
 {
	// TODO Auto-generated constructor stub

}

Island::~Island() {
	// TODO Auto-generated destructor stub
}


void Island::registerToIsland(node::Node *node){members.push_back(node);}
void Island::unregisterToIsland(node::Node *node){members.remove(node);}

void Island::sendMessage(const struct net_address *to_addr,
                 union mesh_internal_msg *msg)
{
	switch(msg->header.msgno) {
	case MSGNO::INVALID:
		m_invalid++;
		break;
	case MSGNO::BROADCAST_ASSOCIATE_REQ:
		m_broadcast_associate_req++;
		break;
	case MSGNO::BROADCAST_ASSOCIATE_RSP:
		m_broadcast_associate_rsp++;
		break;
	case MSGNO::BROADCAST_NEIGHBOUR_REQ:
		m_broadcast_neighbour_req++;
		break;
	case MSGNO::BROADCAST_NEIGHBOUR_RSP:
		m_broadcast_neighbour_rsp++;
		break;
	case MSGNO::NETWORK_ASSIGNMENT_REQ:
		m_network_assignment_req++;
		break;
	case MSGNO::NETWORK_ASSIGNMENT_RSP:
		m_network_assignment_rsp++;
		break;
	case MSGNO::REGISTER_TO_MASTER_REQ:
		m_register_to_master_req++;
		break;
	case MSGNO::REGISTER_TO_MASTER_RSP:
		m_register_to_master_rsp++;
		break;
	case MSGNO::MESSAGE_REQ:
		m_message_req++;
		break;
	case MSGNO::MESSAGE_RSP:
		m_message_rsp++;
		break;

	}
//	printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
	switch(msg->header.msgno) {
	case MSGNO::BROADCAST_ASSOCIATE_REQ: {
		/* Avoid that the node with temporary address also
		 * gets the broadcast message.
		 * Therefore create a list with all receivers except the one
		 * who sent it.
		 */
		std::list<node::Node*> sendMembers;
		// This will be a broadcast message
		for(node::Node* member : members){
			struct net_address addr;
			member->nw->getAddr(&addr);
			// The one sending a broadcast can't also receive it.
			if(NetHelper::compare_net_address(&addr,  &msg->associate_req.from_addr)) continue;
			sendMembers.push_back(member);

		}
		// List with excluded member for the broadcast message.
		for(node::Node* member : sendMembers){
			struct net_address addr;
			member->nw->getAddr(&addr);
//			printf("S-----\n");
//			printf("mem: %p\n", (void*) this);
//			printf("MSGNO: %s\n", NetHelper::getMsgno(msg->header.msgno));
//			printf("%s: addr: ", member->getName());
//			NetHelper::printf_address(&addr);
//			printf("to: ");
//			NetHelper::printf_address(to_addr);
//			printf("E-----\n");

			member->nw->recv_from(msg);

		}
	}
	break;
	default:
//		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
		for(node::Node* member : members){
			struct net_address addr;
			member->nw->getAddr(&addr);
			if(NetHelper::compare_net_address(to_addr, &addr)) {
//				printf("S-----\n");
//				printf("mem: %p\n", (void*) this);
//				printf("MSGNO: %s\n", NetHelper::getMsgno(msg->header.msgno));
//				printf("%s: addr: ", member->getName());
//				NetHelper::printf_address(&addr);
//				printf("destination: ");
//				NetHelper::printf_address(&msg->reg_master_rsp.destination);
//				printf("E-----\n");
//				printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
				member->nw->recv_from(msg);
			}
		}
		break;
	case MSGNO::INVALID:
		printf("FILE: %s, FUNCTION: %s, LINE: %d\n", __FILE__, __FUNCTION__, __LINE__);
		break;
	}
}
void Island::reset() {
	members.clear();
	m_invalid = 0;
	m_broadcast_associate_req = 0;
	m_broadcast_associate_rsp = 0;
	m_broadcast_neighbour_req = 0;
	m_broadcast_neighbour_rsp = 0;
	m_network_assignment_req = 0;
	m_network_assignment_rsp = 0;
	m_register_to_master_req = 0;
	m_register_to_master_rsp = 0;
	m_message_req = 0;
	m_message_rsp = 0;


}
} /* namespace node */
