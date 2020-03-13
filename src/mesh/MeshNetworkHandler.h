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
#include "NetworkInterfaceSubscriber.h"

union mesh_internal_msg;
struct net_address;
struct node_data;

namespace mesh {
class NetworkData;
class Mesh;
}

namespace NetAlgorithm {
class NetAlgorithmInterface;
}

namespace network {
class NetworkInterface;
}

namespace mesh {
class MeshNetworkHandler : network::NetworkInterfaceSubscriber{
private:
	Mesh *mesh;
	NetworkData *network;  /*!< Detailed description after the member */
	network::NetworkInterface *nw;
	NetAlgorithm::NetAlgorithmInterface *algorithm;

public:
	MeshNetworkHandler(Mesh *mesh, NetworkData *network,
	                   network::NetworkInterface *nw,
	                   NetAlgorithm::NetAlgorithmInterface *algorithm);
	virtual ~MeshNetworkHandler();

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

	/* Mesh related */
	void doBroadcastAssociateReq();
	void doBroadcastAssociateRsp(union mesh_internal_msg *msg);
	void doRegisterReq();
	void doPingParentReq();
	void doRegisterToMasterRsp(union mesh_internal_msg *msg);
	int doChooseParent();
	void doRegisterToMasterReq(union mesh_internal_msg *msg);
	void doNetworkAssignmentRsp(union mesh_internal_msg *msg);
	void doDisconnectChildReq(struct node_data *node);

};

} /* namespace mesh */

