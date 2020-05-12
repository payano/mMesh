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

#include "MeshNetworkHandler.h"
#include "DataTypes.h"
#include "NetworkData.h"
#include "Mesh.h"
#include "NetAlgorithmInterface.h"
#include "NetworkData.h"
#include "NetworkInterface.h"
#include "SyscallsInterface.h"

namespace mesh {

MeshNetworkHandler::MeshNetworkHandler(NetworkData *network,
                                       network::NetworkInterface *nw,
                                       NetAlgorithm::NetAlgorithmInterface *algorithm):
			network(network), nw(nw), algorithm(algorithm) {
	nw->registerSubscriber(this);
	// TODO Auto-generated constructor stub

}

MeshNetworkHandler::~MeshNetworkHandler() {
	// TODO Auto-generated destructor stub
}

int MeshNetworkHandler::getSubnetToChild(struct net_address *address)
{
	int ret = 0;
	// init subnet
	syscalls::SyscallsInterface::mem_clr(address, sizeof(*address));

	ret = network->generate_child_address(address);
	return ret;
}

void MeshNetworkHandler::network_recv(union mesh_internal_msg *msg) {
	/* These methods will be run from the interrupt vector
	 * They all they is doing is adding the messages to the message queue
	 * If the message destination is to this node.
	 */

	switch(msg->header.msgno) {
	case MSGNO::BROADCAST_ASSOCIATE_REQ:
		if(false == network->registeredToMaster || network->pairedChildren >= CHILDREN_SZ) {
			return;
		}
		network->queue_add(msg);
		break;
	case MSGNO::BROADCAST_ASSOCIATE_RSP:
		network->queue_add(msg);
		break;
	case MSGNO::NETWORK_ASSIGNMENT_REQ:
		network->queue_add(msg);
		break;
	case MSGNO::NETWORK_ASSIGNMENT_RSP:
		network->queue_add(msg);
		break;
	case MSGNO::REGISTER_TO_MASTER_REQ:
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
			return;
		}
		network->queue_add(msg);
		break;
	case MSGNO::REGISTER_TO_MASTER_RSP:
		// Drop packet. not valid anymore.
		msg->header.hop_count++;
		if(msg->header.hop_count >= MAX_HOPS){
			return;
		}

		// I'm not master, pass it along
		// Here we should do routing algorithm
		if(syscalls::SyscallsInterface::cmp_data(&network->mac, &msg->reg_master_rsp.destination,
		                                         sizeof(network->mac))){
			const struct net_address *dst = algorithm->getRouteForPacket(
					network, &msg->reg_master_rsp.destination);
			nw->sendto(dst, msg);
			return;
		}
		network->queue_add(msg);
		break;
	case PING_PARENT_REQ:
		// If it is our child, respond.
		if(!algorithm->is_child_of(&network->mac, &msg->ping_parent_req.from))
			return;
		network->queue_add(msg);
		break;
	case PING_PARENT_RSP:
		// Check that is ours
		if(syscalls::SyscallsInterface::cmp_data(&network->mac, &msg->ping_parent_rsp.to,
		                                         sizeof(network->mac)))
			return;
		network->queue_add(msg);
		break;
	case MSGNO::DISCONNECT_CHILD_REQ:
		//Check parent
		if(syscalls::SyscallsInterface::cmp_data(&network->parent.mac, &msg->disconnect_child_req.from,
		                                         sizeof(network->parent.mac))) return;

		network->queue_add(msg);
		break;
	case MSGNO::BROADCAST_NEIGHBOUR_REQ:
		//		printf("%s: _ BROADCAST_NEIGHBOUR_REQ\n", mesh->getName());
		if(network->registeredToMaster)
			network->queue_add(msg);
		break;
	case MSGNO::BROADCAST_NEIGHBOUR_RSP:
		//		printf("%s: _ BROADCAST_NEIGHBOUR_RSP\n", mesh->getName());
		if(network->registeredToMaster)
			network->queue_add(msg);
		break;
	case MSGNO::PING_NEIGHBOUR_REQ:
		if(network->registeredToMaster)
			network->queue_add(msg);
		//		printf("%s: _ PING_NEIGHBOUR_REQ\n", mesh->getName());
		break;
	case MSGNO::PING_NEIGHBOUR_RSP:
		//		printf("%s: _ PING_NEIGHBOUR_RSP\n", mesh->getName());
		if(network->registeredToMaster)
			network->queue_add(msg);
		break;
	case MSGNO::DISCONNECT_CHILD_RSP:
	case MSGNO::MESSAGE_REQ:
	case MSGNO::MESSAGE_RSP:
	case MSGNO::INVALID:
		break;
	}
}

void MeshNetworkHandler::doBroadcastAssociateReq(){
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::BROADCAST_ASSOCIATE_REQ;
	msg.header.hop_count = 0;
	syscalls::SyscallsInterface::copy_data(&msg.associate_req.from_addr, &network->mac,
	                                       sizeof(msg.associate_req.from_addr));
	nw->sendto(&BROADCAST, &msg);
}

void MeshNetworkHandler::doBroadcastAssociateRsp(union mesh_internal_msg *msg)
{
	union mesh_internal_msg rsp;
	rsp.header.hop_count = 0;
	rsp.header.msgno = MSGNO::BROADCAST_ASSOCIATE_RSP;
	syscalls::SyscallsInterface::copy_data(&rsp.associate_rsp.parent_address, &network->mac,
	                                       sizeof(rsp.associate_rsp.parent_address));
	nw->sendto(&msg->associate_req.from_addr, &rsp);
}

void MeshNetworkHandler::doRegisterReq(){
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::REGISTER_TO_MASTER_REQ;
	msg.header.hop_count = 0;
	//	printf("%s: doing doRegisterReq: ", mesh->getName());
	//	NetHelper::printf_address(&network->mac);
	syscalls::SyscallsInterface::copy_data(&msg.reg_master_req.host_addr, &network->mac,
	                                       sizeof(msg.reg_master_req.host_addr));
	nw->sendto(&network->parent.mac, &msg);
}

void MeshNetworkHandler::doPingParentReq()
{
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::PING_PARENT_REQ;
	msg.header.hop_count = 0;
	syscalls::SyscallsInterface::copy_data(&msg.ping_parent_req.from, &network->mac, sizeof(network->mac));
	nw->sendto(&network->parent.mac, &msg);
}

void MeshNetworkHandler::doRegisterToMasterRsp(union mesh_internal_msg *msg)
{
	union mesh_internal_msg rsp;
	const struct net_address *dst;
	rsp.header.hop_count = 0;
	rsp.reg_master_rsp.header.msgno = MSGNO::REGISTER_TO_MASTER_RSP;
	rsp.reg_master_rsp.status = STATUS::OK;

	syscalls::SyscallsInterface::copy_data(&rsp.reg_master_rsp.destination, &msg->reg_master_req.host_addr,
	                                       sizeof(rsp.reg_master_rsp.destination));

	dst = algorithm->getRouteForPacket(network,
	                                   &msg->reg_master_req.host_addr);
	nw->sendto(dst, &rsp);
}

int MeshNetworkHandler::doChooseParent()
{
	struct net_address parent;
	int ret = algorithm->choose_parent_from_list(network, &parent);

	if(ret) {
		return -1 ;
	}

	union mesh_internal_msg rsp;
	rsp.header.msgno = MSGNO::NETWORK_ASSIGNMENT_REQ;
	rsp.header.hop_count = 0;
	syscalls::SyscallsInterface::copy_data(&rsp.associate_req.from_addr, &network->mac,
	                                       sizeof(network->mac));

	nw->sendto(&parent, &rsp);
	return 0;

}

void MeshNetworkHandler::doRegisterToMasterReq(union mesh_internal_msg *msg)
{
	struct network_assignment_rsp *new_msg = &msg->assignment_rsp;
	syscalls::SyscallsInterface::copy_data(&network->parent.mac, &new_msg->parent, sizeof(new_msg->parent));
	syscalls::SyscallsInterface::copy_data(&network->mac, &new_msg->new_address,
	                                       sizeof(new_msg->new_address));
	nw->setAddr(&network->mac);
	network->mPaired = true;
}

void MeshNetworkHandler::doNetworkAssignmentRsp(union mesh_internal_msg *msg)
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
	syscalls::SyscallsInterface::copy_data(&rsp.assignment_rsp.parent, &network->mac, sizeof(network->mac));
	nw->sendto(&msg->assignment_req.from_address, &rsp);
	//	printf("SENDING TO: ");
	//	NetHelper::printf_address(&msg->assignment_req.from_address);
}

void MeshNetworkHandler::doDisconnectChildReq(struct node_data *node)
{
	/* We want to disconnect the node.
	 * because we don't consider it to be online any more.
	 */
	struct net_address remove_node;
	int ret = network->remove_child_node(node, &remove_node);
	if(ret) return;

	union mesh_internal_msg msg;
	msg.header.hop_count = 0;
	// We dont want a respond for this message.
	msg.header.msgno = MSGNO::DISCONNECT_CHILD_REQ;
	syscalls::SyscallsInterface::copy_data(&msg.disconnect_child_req.from, &network->mac,
	                                       sizeof(network->mac));
	syscalls::SyscallsInterface::copy_data(&msg.disconnect_child_req.to, &remove_node, sizeof(remove_node));
	nw->sendto(&remove_node, &msg);
}

void MeshNetworkHandler::doPingParentRsp(union mesh_internal_msg *msg)
{
	//	updateParentCounters();
	union mesh_internal_msg rsp;
	rsp.header.hop_count = 0;
	rsp.reg_master_rsp.header.msgno = MSGNO::PING_PARENT_RSP;
	rsp.reg_master_rsp.status = STATUS::OK;

	syscalls::SyscallsInterface::copy_data(&rsp.ping_parent_rsp.to, &msg->ping_parent_req.from,
	                                       sizeof(msg->ping_parent_req.from));
	syscalls::SyscallsInterface::copy_data(&rsp.ping_parent_rsp.from, &network->mac, sizeof(network->mac));

	nw->sendto(&rsp.ping_parent_rsp.to, &rsp);
}

void MeshNetworkHandler::doSeekNeighbours()
{
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::BROADCAST_NEIGHBOUR_REQ;
	msg.header.hop_count = 0;
	syscalls::SyscallsInterface::copy_data(&msg.neighbour_req.from, &network->mac, sizeof(network->mac));
	nw->sendto(&BROADCAST, &msg);
}

void MeshNetworkHandler::doNeighborRsp(union mesh_internal_msg *msg)
{
	union mesh_internal_msg rsp;
	rsp.header.msgno = MSGNO::BROADCAST_NEIGHBOUR_RSP;
	rsp.header.hop_count = 0;
	syscalls::SyscallsInterface::copy_data(&rsp.neighbour_rsp.net_address, &network->mac,
	                                       sizeof(network->mac));
	nw->sendto(&msg->neighbour_req.from, &rsp);
}

void MeshNetworkHandler::doPingNeighbours()
{
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::PING_NEIGHBOUR_REQ;
	msg.header.hop_count = 0;
	for(int i = 0; i < NEIGHBOUR_SZ; ++i){
		if(!network->neighbours[i].connected) continue;
		syscalls::SyscallsInterface::copy_data(&msg.ping_neighbour_req.from, &network->mac,
		                                       sizeof(network->mac));
		syscalls::SyscallsInterface::copy_data(&msg.ping_neighbour_req.to, &network->neighbours[i].mac,
		                                       sizeof(network->mac));
		nw->sendto(&network->neighbours[i].mac, &msg);
	}
}

void MeshNetworkHandler::doPingNeighbourRsp(union mesh_internal_msg *msg)
{
	union mesh_internal_msg rsp;
	rsp.header.msgno = MSGNO::PING_NEIGHBOUR_RSP;
	rsp.header.hop_count = 0;
	syscalls::SyscallsInterface::copy_data(&rsp.ping_neighbour_rsp.from, &network->mac,
	                                       sizeof(network->mac));
	syscalls::SyscallsInterface::copy_data(&rsp.ping_neighbour_rsp.to, &msg->ping_neighbour_req.from,
	                                       sizeof(msg->ping_neighbour_req.from));
	nw->sendto(&msg->ping_neighbour_req.from, &rsp);
}

} /* namespace mesh */
