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

namespace NetAlgorithm {

LazyAlgorithm::LazyAlgorithm() {
	// TODO Auto-generated constructor stub

}

LazyAlgorithm::~LazyAlgorithm() {
	// TODO Auto-generated destructor stub
}

const struct net_address *LazyAlgorithm::getRouteForPacket(const struct networkData *nw,
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
		bool childOf = NetHelper::isChildOf(&nw->childs[i].mac, to);
		if(true == childOf) {
			return &nw->childs[i].mac;
		}
		if(true == NetHelper::compare_net_address(to, &nw->childs[i].mac)){
			return to;
		}
	}
	return &nw->parent.mac;
}

int LazyAlgorithm::associate_rsp_add_parent_to_list(struct networkData *nw,
                                                    union mesh_internal_msg *msg)
{
	NetHelper::queue_add(nw, msg);
	return 0;
}
int LazyAlgorithm::choose_parent_from_list(struct networkData *nw,
                                                 struct net_address *parent)
{
	// Choose the right one to associate with.
	// We take the first one and discard the rest.
	union mesh_internal_msg queue_msg;
	NetHelper::queue_get(nw, &queue_msg);
	NetHelper::queue_clear(nw);

	if(MSGNO::BROADCAST_ASSOCIATE_RSP != queue_msg.header.msgno){
		// This is error
		return -1;
	}

	NetHelper::copy_net_address(parent, &queue_msg.associate_rsp.parent_address);

	return 0;
	}

} /* namespace NetAlgorithm */
