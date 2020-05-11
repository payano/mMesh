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
#include "SyscallsInterface.h"
namespace mesh {

NetworkData::NetworkData() {

}

NetworkData::NetworkData(syscalls::SyscallsInterface *syscalls) {
	this->syscalls = syscalls;
	mem_clr(&mac, sizeof(mac));
	mem_clr(&parent.mac, sizeof(parent.mac));
	pairedChildren = 0;
	pairedNeighbours = 0;
	registeredToMaster = false;
	mPaired = false;
	buffer_count = 0;
	for(int i = 0 ; i < CHILDREN_SZ; ++i) {
		mem_clr(&childs[i], sizeof(childs[i]));
	}
	for(int i = 0 ; i < NEIGHBOUR_SZ; ++i) {
		mem_clr(&neighbours[i], sizeof(neighbours[i]));
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
	copy_data(&queuedmsgs[buffer_count++], msg, sizeof(*msg));
	return 0;
}

int NetworkData::queue_get(union mesh_internal_msg *msg)
{
	/* take first one, move all one stop the the left in the queue
	 * for example: [1] -> [0]
	 */
	if(buffer_count <= 0) return 1;

	copy_data(msg, &queuedmsgs[0], sizeof(*msg));

	buffer_count--;
	for(int i = 0 ; i < buffer_count; ++i) {
		copy_data(&queuedmsgs[i], &queuedmsgs[i+1], sizeof(queuedmsgs[i]));
	}

	queuedmsgs[buffer_count].header.msgno = MSGNO::INVALID;
	return 0;
}

int NetworkData::remove_child_node(struct node_data *node,
                                   struct net_address *disband_node)
{
	struct node_data *child = childs;
	for(int i = 0; i < CHILDREN_SZ; ++i, child++) {
		// NEEDED?
		if(false == child->connected) continue;
		if(cmp_data(&node->mac, &child->mac, sizeof(node->mac))) continue;
		mem_clr(disband_node, sizeof(*disband_node));

		copy_data(disband_node, &child->mac, sizeof(child->mac));
		child->keepalive_count = 0;
		mem_clr(&child->mac, sizeof(child->mac));
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
	}
}

void NetworkData::decrease_nb_timers(uint value)
{
	struct node_data *nwd_nb = neighbours;
	for(int i = 0; i < CHILDREN_SZ; ++i, ++nwd_nb) {
		if(false == nwd_nb->connected) continue;
		if(nwd_nb->keepalive_count <= 0) {
			nwd_nb->keepalive_count = 0;
			continue;
		}
		nwd_nb->keepalive_count -= value;
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

int NetworkData::iterateNeighbours(struct node_data **node)
{
	/* one past last is okay, but not beyond that */
	const struct node_data *last = &neighbours[NEIGHBOUR_SZ];
	if(nullptr == *node){
		*node = neighbours;
		return 1;
	}
	++(*node);
	return *node == last ? 0 : 1;
}

int NetworkData::generate_child_address(struct net_address *address)
{
	mem_clr(address, sizeof(*address));
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
	copy_data(&childs[child_addr-1].mac, address, sizeof(*address));
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
	if(cmp_data(node,&parent.mac, sizeof(parent.mac))) return;
	parent.keepalive_count = TIMER_KEEPALIVE;
}

void NetworkData::updateNeighbourCounter(const struct net_address *node)
{
	struct node_data *nb = findNeighbour(node);
	if(nullptr == nb) return;
	nb->keepalive_count = TIMER_KEEPALIVE;
}

struct node_data *NetworkData::findChild(const struct net_address *child)
{
	struct node_data *nwd_child = childs;
	for(int i = 0; i < CHILDREN_SZ; ++i, ++nwd_child) {
		if(false == nwd_child->connected) continue;
		if(cmp_data(child, &nwd_child->mac, sizeof(*child))) continue;
		return nwd_child;
	}
	return nullptr;
}

struct node_data *NetworkData::findNeighbour(const struct net_address *neighbour)
{
	struct node_data *nwd_nb = neighbours;
	for(int i = 0; i < CHILDREN_SZ; ++i, ++nwd_nb) {
		if(false == nwd_nb->connected) continue;
		if(cmp_data(neighbour, &nwd_nb->mac, sizeof(*neighbour))) continue;
		return nwd_nb;
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


void NetworkData::generate_temporary_address(struct net_address *address) {
	address->broadcast = 0;
	address->master = 0;
	address->gen_addr = 1;
	for(int i = 0 ; i < NET_COUNT; ++i) {
		address->nbs[i].net = generate_number(NUM_ADDRESSES);
	}
	address->host_addr = generate_number(NUM_ADDRESSES);
}

uint8_t NetworkData::generate_number(int max_number){
	return syscalls->get_random() % max_number;
}

/* Should be moved to a class*/
bool NetworkData::check_timer_zero(struct node_data *node){
	return node->keepalive_count == 0 ? true : false;
}

/* Should be moved to a class*/
bool NetworkData::checkConnected(struct node_data *node){
	return node->connected == true ? true : false;
}


}
