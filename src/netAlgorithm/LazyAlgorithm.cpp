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

#include "LazyAlgorithm.h"
#include "DataTypes.h"
#include "NetworkData.h"

namespace NetAlgorithm {

LazyAlgorithm::LazyAlgorithm() : rank(2){
	// TODO Auto-generated constructor stub

}

LazyAlgorithm::~LazyAlgorithm() {
	// TODO Auto-generated destructor stub
}

const struct net_address *LazyAlgorithm::getRouteForPacket(const mesh::NetworkData *nw,
                                                           const struct net_address *to)
{
	/* We can also involves costs for each route, but we are a lazy algorithm
	 * and only showing a proof of concept for now.
	 */

	/* Check if we can take a shortcut with our neighbors */
	for(int i = 0 ; i < NEIGHBOUR_SZ ; ++i){
		if(!nw->neighbours[i].connected) continue;
		bool nb_parent = is_child_of(&nw->neighbours[i].mac, to, false);
		bool to_parent = is_child_of(to, &nw->neighbours[i].mac, false);
		bool same_addr = !cmp_data(&nw->neighbours[i].mac, to, sizeof(*to));

		if(nb_parent || to_parent || same_addr)
			return &nw->neighbours[i].mac;
	}

	/* Check childs's */
	if(0x1 == nw->mac.master) {
		for(int i = 0 ; i < CHILDREN_SZ; ++i){
			if(nw->childs[i].mac.nbs[0].net == to->nbs[0].net)
				return &nw->childs[i].mac;
		}
	}

	for(int i = 0; i < nw->pairedChildren; ++i) {
		bool childOf = is_child_of(&nw->childs[i].mac, to);

		if(true == childOf)
			return &nw->childs[i].mac;

		if(!cmp_data(to, &nw->childs[i].mac, sizeof(*to)))
			return to;
	}

	return &nw->parent.mac;
}

int LazyAlgorithm::associate_rsp_add_parent_to_list(mesh::NetworkData *nw,
                                                    union mesh_internal_msg *msg)
{
	nw->queue_add(msg);
	return 0;
}
int LazyAlgorithm::choose_parent_from_list(mesh::NetworkData *nw,
                                                 struct net_address *parent)
{
	// Choose the right one to associate with.
	// We take the first one and discard the rest.
	union mesh_internal_msg queue_msg;
	nw->queue_get(&queue_msg);
	nw->queue_clear();

	if(MSGNO::BROADCAST_ASSOCIATE_RSP != queue_msg.header.msgno){
		// This is error
		return -1;
	}

	copy_data(parent, &queue_msg.associate_rsp.parent_address, sizeof(*parent));
	return 0;
}

int LazyAlgorithm::is_child_of(const struct net_address *parent,
                               const struct net_address *child,
                               bool include_master)
{
	if(!include_master && parent->master) return false; // all is childs to master
	if(parent->master)                    return true;

	for(int i = 0; i < NET_COUNT; ++i) {
		if(parent->nbs[i].net == child->nbs[i].net) continue;
		if(parent->nbs[i].net == 0x0) return true;
		break;
	}
	return false;
}

void LazyAlgorithm::set_minimum_rank(int rank) {this->rank = rank;}

int LazyAlgorithm::get_address_depth(const struct net_address *nb_address)
{
	if(0x1 == nb_address->master) return 0;

	for(int i = 0; i < NET_COUNT; ++i)
		if(nb_address->nbs[i].net == 0x0) return i;

	return NET_COUNT;
}

int LazyAlgorithm::get_common_ancestor(const struct net_address *mac,
                                       const struct net_address *nb_address)
{
	for(int i = 0; i <= NET_COUNT ; ++i)
		if(mac->nbs[i].net != nb_address->nbs[i].net) return i;

	return NET_COUNT;
}


int LazyAlgorithm::evaluate_nb_address(mesh::NetworkData *nw,
                                       const struct net_address *nb_address)
{
	if(nw->pairedNeighbours == NEIGHBOUR_SZ) return 0;

	// Sort out which level we are.
	int our_level = get_address_depth(&nw->mac);
	// Sort out which level nb_address is.
	int nb_level = get_address_depth(nb_address);
	// Get common ancestor
	int ancestor = get_common_ancestor(&nw->mac, nb_address);

	// Algorithm is:
	int rank = our_level + nb_level - 2 * ancestor;

	if(rank <= this->rank) return 0;

	/* 1 means that this node should be added.
	* if there is space left in the neighbour list.
	* We could also check if we want to overwrite another node with lower rank.
	* But this is the lazy algorithm, we are not a smart one, just a lazy one.
	*/

	for(int i = 0; i < NEIGHBOUR_SZ; ++i) {
		if(nw->neighbours[i].connected) continue;

		copy_data(&nw->neighbours[nw->pairedNeighbours].mac, nb_address,
		          sizeof(*nb_address));
		nw->neighbours[i].connected = true;
		nw->pairedNeighbours++;
		return 1;
	}

	// should never occur
	return 0;
}

} /* namespace NetAlgorithm */
