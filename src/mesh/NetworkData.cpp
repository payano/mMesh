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

#include "NetworkData.h"

namespace mesh {

NetworkData::NetworkData() {
	NetHelper::clear_net_address(&mac);
	NetHelper::clear_net_address(&parent.mac);
	pairedChildren = 0;
	registeredToMaster = false;
	mPaired = false;
	buffer_count = 0;

	for(int i = 0 ; i < CHILDREN_SZ; ++i) {
		NetHelper::clear_net_address(&childs[i].mac);
		childs[i].keepalive_count = 0;
		childs[i].connected = false;
	}
	// TODO Auto-generated constructor stub

}

NetworkData::~NetworkData() {
	// TODO Auto-generated destructor stub
}

int NetworkData::queue_sz() { return buffer_count; }
void NetworkData::queue_clear(){buffer_count = 0;}
int NetworkData::queue_add(union mesh_internal_msg *msg)
{
	if(buffer_count >= MSG_BUFFER) return -1;
	NetHelper::copy_internal_msg(&queuedmsgs[buffer_count++], msg);
	return 0;
}

int NetworkData::queue_get(union mesh_internal_msg *msg)
{
	/* take first one, move all one stop the the left in the queue
	 * for example: [1] -> [0]
	 */
	if(0 == buffer_count) return 1;

	NetHelper::copy_internal_msg(msg, &queuedmsgs[0]);

	buffer_count--;
	for(int i = 0 ; i < buffer_count; ++i) {
		NetHelper::copy_internal_msg(&queuedmsgs[i], &queuedmsgs[i+1]);
	}

	return 0;
}

int NetworkData::remove_child_node(struct node_data *node,
                                   struct net_address *disband_node)
{
	struct node_data *child = childs;
	for(int i = 0; i < CHILDREN_SZ; ++i, child++) {
		// NEEDED?
		if(false == child->connected) continue;
		if(!NetHelper::compare_net_address(&node->mac, &child->mac)) continue;
		NetHelper::clear_net_address(disband_node);

		NetHelper::copy_net_address(disband_node, &child->mac);
		child->keepalive_count = 0;
		NetHelper::clear_net_address(&child->mac);
		child->connected = false;
		pairedChildren--;
		return 0;
	}
	return -1;
}

void NetworkData::decrease_parent_timer(uint value)
{
	if(parent.keepalive_count <= 0){
		parent.keepalive_count = 0;
		return;
	}
	parent.keepalive_count -= value;
}

void NetworkData::decrease_child_timers(uint value)
{
	struct node_data *nwd_child = childs;
	for(int i = 0; i < CHILDREN_SZ; ++i, ++nwd_child) {
		if(false == nwd_child->connected) continue;
		if(nwd_child->keepalive_count <= 0) {
			nwd_child->keepalive_count = 0;
			continue;
		}
		nwd_child->keepalive_count -= value;
//			printf_address(&nwd_child->mac);
//			printf("KEEPALIVE: %d\n",nwd_child->keepalive_count);
	}
}

int NetworkData::iterateChilds(struct node_data **node)
{
	/* one past last is okay, but not beyond that */
	const struct node_data *last = &childs[CHILDREN_SZ];
	if(nullptr == *node){
		*node = childs;
		return 1;
	}
	++(*node);
	return *node == last ? 0 : 1;
}

int NetworkData::generate_child_address(struct net_address *address)
{
	NetHelper::clear_net_address(address);
	int ret;
	int child_addr = -1;
	for(int i = 0; i < CHILDREN_SZ; ++i){
		if(childs[i].connected == false) {
			// Found a free slot
			child_addr = i;
			break;
		}
	}

	// No addresses free.. here we might need to release one.
	if(child_addr < 0) {return -1;}
	// Assign it
	childs[child_addr].connected = true;
	childs[child_addr].keepalive_count = TIMER_KEEPALIVE;

	// When using it we want to start with number 1 not 0
	child_addr++;
	pairedChildren++;
	// Get new address
	ret = getNewChildAddress(&mac, address, child_addr);
	if(ret) {
		return -1;
	}
	// Add it to parent neighbour list
	NetHelper::copy_net_address(&childs[child_addr-1].mac, address);
	return 0;
}


void NetworkData::updateChildCounter(const struct net_address *node)
{
	struct node_data *child = findChild(node);
	if(nullptr == child) return;
	child->keepalive_count = TIMER_KEEPALIVE;
}

void NetworkData::updateParentCounter(const struct net_address *node)
{
	if(!NetHelper::compare_net_address(node,&parent.mac)) return;
	parent.keepalive_count = TIMER_KEEPALIVE;
}

struct node_data *NetworkData::findChild(const struct net_address *child)
{
	struct node_data *nwd_child = childs;
	for(int i = 0; i < CHILDREN_SZ; ++i, ++nwd_child) {
		if(false == nwd_child->connected) continue;
		if(!NetHelper::compare_net_address(child, &nwd_child->mac)) continue;
		return nwd_child;
	}
	return nullptr;
}

int NetworkData::getNewChildAddress(const struct net_address *parent,
                                    struct net_address *child,
                                    const int childId)
{
	if(parent->master) {
		// Easy one
		child->nbs[0].net = childId;
		return 0;
	}
	// here we need to find the parent, also copying the bytes from parent
	for(int i = 0 ; i < NET_COUNT ; ++i) {
		child->nbs[i].net = parent->nbs[i].net;
		if(parent->nbs[i].net == 0) {
			child->nbs[i].net = childId;
			return 0;
		}
	}
	return -1;
}

}
