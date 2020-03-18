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

LazyAlgorithm::LazyAlgorithm() {
	// TODO Auto-generated constructor stub

}

LazyAlgorithm::~LazyAlgorithm() {
	// TODO Auto-generated destructor stub
}

const struct net_address *LazyAlgorithm::getRouteForPacket(const mesh::NetworkData *nw,
                                                           const struct net_address *to)
{
	/* Check nb's */
	if(1 == nw->mac.master) {
		for(int i = 0 ; i < CHILDREN_SZ; ++i){
			if(nw->childs[i].mac.nbs[0].net == to->nbs[0].net) {
				return &nw->childs[i].mac;
			}
		}
	}

	for(int i = 0; i < nw->pairedChildren; ++i) {
		bool childOf = isChildOf(&nw->childs[i].mac, to);
		if(true == childOf) {
			return &nw->childs[i].mac;
		}
		if(!cmp_data(to, &nw->childs[i].mac, sizeof(*to))){
			return to;
		}
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

int LazyAlgorithm::isChildOf(const struct net_address *parent,
                             const struct net_address *child)
{
	if(parent->master) return true; // all is childs to master

	for(int i = 0; i < NET_COUNT; ++i) {
		if(parent->nbs[i].net == child->nbs[i].net) continue;
		if(parent->nbs[i].net == 0x0) {return true;}
		break;
	}
	return false;
}
} /* namespace NetAlgorithm */

