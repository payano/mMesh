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

namespace mesh {

MeshNetworkHandler::MeshNetworkHandler(Mesh *mesh, NetworkData *network,
                                       network::NetworkInterface *nw,
                                       NetAlgorithm::NetAlgorithmInterface *algorithm):
	mesh(mesh), network(network), nw(nw), algorithm(algorithm) {
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
	NetHelper::clear_net_address(address);

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
//		printf("%s: _ PING_PARENT_REQ\n", mesh->getName());
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

void MeshNetworkHandler::handle_associate_rsp(union mesh_internal_msg *msg) {
//	printf("%s: BROADCAST_ASSOCIATE_RSP\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_associate_req(union mesh_internal_msg *msg){
	if(false == network->mPaired || network->pairedChildren >= CHILDREN_SZ) {
		return;
	}
//	printf("%s: BROADCAST_ASSOCIATE_REQ\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_network_assignment_req(union mesh_internal_msg *msg){
//	printf("%s: NETWORK_ASSIGNMENT_REQ\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_network_assignment_rsp(union mesh_internal_msg *msg){
//	printf("%s: NETWORK_ASSIGNMENT_RSP\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_register_to_master_req(union mesh_internal_msg *msg){
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
//		printf("%s: ROUTING REGISTER_TO_MASTER_REQ\n", mesh->getName());
		return;
	}

//	printf("%s: REGISTER_TO_MASTER_REQ\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_register_to_master_rsp(union mesh_internal_msg *msg){

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

//		printf("%s: me : ", mesh->getName());
//		NetHelper::printf_address(&network->mac);
//		printf("%s: ROUTING REGISTER_TO_MASTER_RSP\n", mesh->getName());
//		printf("%s: HEADER DEST: ", mesh->getName());
//		NetHelper::printf_address(&msg->reg_master_rsp.destination);
//		printf("%s: ROUTING DST: ", mesh->getName());
//		NetHelper::printf_address(dst);
		return;
	}

	printf("%s: REGISTER_TO_MASTER_RSP\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_ping_parent_req(union mesh_internal_msg *msg)
{
	// If it is our child, respond.
	if(!algorithm->isChildOf(&network->mac, &msg->ping_parent_req.from))
		return;
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_ping_parent_rsp(union mesh_internal_msg *msg)
{
	// Check that is ours
	if(!NetHelper::compare_net_address(&network->mac, &msg->ping_parent_rsp.to))
		return;
//	printf("%s: handle_ping_parent_rsp\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::handle_disconnect_req(union mesh_internal_msg *msg)
{

	//Check parent
	if(!NetHelper::compare_net_address(&network->parent.mac,
	                               &msg->disconnect_child_req.from)) return;

//	printf("%s: handle_ping_parent_rsp\n", mesh->getName());
	network->queue_add(msg);
}

void MeshNetworkHandler::doBroadcastAssociateReq(){
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::BROADCAST_ASSOCIATE_REQ;
	msg.header.hop_count = 0;
	NetHelper::copy_net_address(&msg.associate_req.from_addr, &network->mac);
	nw->sendto(&BROADCAST, &msg);
}

void MeshNetworkHandler::doBroadcastAssociateRsp(union mesh_internal_msg *msg)
{
	union mesh_internal_msg rsp;
	rsp.header.hop_count = 0;
	rsp.header.msgno = MSGNO::BROADCAST_ASSOCIATE_RSP;
	NetHelper::copy_net_address(&rsp.associate_rsp.parent_address, &network->mac);
	nw->sendto(&msg->associate_req.from_addr, &rsp);
}

void MeshNetworkHandler::doRegisterReq(){
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::REGISTER_TO_MASTER_REQ;
	msg.header.hop_count = 0;
//	printf("%s: doing doRegisterReq: ", mesh->getName());
//	NetHelper::printf_address(&network->mac);
	NetHelper::copy_net_address(&msg.reg_master_req.host_addr, &network->mac);
	nw->sendto(&network->parent.mac, &msg);
}

void MeshNetworkHandler::doPingParentReq()
{
	union mesh_internal_msg msg;
	msg.header.msgno = MSGNO::PING_PARENT_REQ;
	msg.header.hop_count = 0;
	NetHelper::copy_net_address(&msg.ping_parent_req.from, &network->mac);
	nw->sendto(&network->parent.mac, &msg);
}

void MeshNetworkHandler::doRegisterToMasterRsp(union mesh_internal_msg *msg)
{
	union mesh_internal_msg rsp;
	const struct net_address *dst;
	rsp.header.hop_count = 0;
	rsp.reg_master_rsp.header.msgno = MSGNO::REGISTER_TO_MASTER_RSP;
	rsp.reg_master_rsp.status = STATUS::OK;

	NetHelper::copy_net_address(&rsp.reg_master_rsp.destination,
	                            &msg->reg_master_req.host_addr);

	dst = algorithm->getRouteForPacket(network,
	                                   &rsp.reg_master_rsp.destination);
	nw->sendto(dst, &rsp);
}

int MeshNetworkHandler::doChooseParent()
{
	struct net_address parent;
	int ret = algorithm->choose_parent_from_list(network, &parent);

	if(ret) {
		return -1 ;
	}

	printf("%s: STARTING_CHOOSING_PARENT\n", mesh->getName());
	union mesh_internal_msg rsp;
	rsp.header.msgno = MSGNO::NETWORK_ASSIGNMENT_REQ;
	rsp.header.hop_count = 0;
	NetHelper::copy_net_address(&rsp.associate_req.from_addr, &network->mac);

	nw->sendto(&parent, &rsp);
	return 0;

}

void MeshNetworkHandler::doRegisterToMasterReq(union mesh_internal_msg *msg)
{
	printf("%s: STARTING_WAITING_FOR_PARENT\n", mesh->getName());
	struct network_assignment_rsp *new_msg = &msg->assignment_rsp;
	NetHelper::copy_net_address(&network->parent.mac, &new_msg->parent);
	NetHelper::copy_net_address(&network->mac, &new_msg->new_address);
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
	NetHelper::copy_net_address(&rsp.assignment_rsp.parent, &network->mac);
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
	NetHelper::copy_net_address(&msg.disconnect_child_req.from, &network->mac);
	NetHelper::copy_net_address(&msg.disconnect_child_req.to, &remove_node);
	nw->sendto(&remove_node, &msg);
}

void MeshNetworkHandler::doPingParentRsp(union mesh_internal_msg *msg)
{
//	updateParentCounters();
	union mesh_internal_msg rsp;
	rsp.header.hop_count = 0;
	rsp.reg_master_rsp.header.msgno = MSGNO::PING_PARENT_RSP;
	rsp.reg_master_rsp.status = STATUS::OK;

	NetHelper::copy_net_address(&rsp.ping_parent_rsp.to,
	                            &msg->ping_parent_req.from);
	NetHelper::copy_net_address(&rsp.ping_parent_rsp.from,
	                            &network->mac);

	nw->sendto(&rsp.ping_parent_rsp.to, &rsp);
}

} /* namespace mesh */
