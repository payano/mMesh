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
#include "Constants.h"
#include "DataTypes.h"

using uint = unsigned int;

namespace mesh {

class NetworkData {
private:
	union mesh_internal_msg queuedmsgs[MSG_BUFFER];
	int buffer_count;
public:
	int pairedChildren;
	int pairedNeighbours;
	bool mPaired;
	bool registeredToMaster;
	struct node_data childs[CHILDREN_SZ];
	struct node_data neighbours[NEIGHBOUR_SZ];
	struct node_data parent;
	net_address mac;

	NetworkData();
	virtual ~NetworkData();

	int queue_sz();
	void queue_clear();
	int queue_add(union mesh_internal_msg *msg);
	int queue_get(union mesh_internal_msg *msg);

	int remove_child_node(struct node_data *node,
	                      struct net_address *disband_node);

	void decrease_parent_timer(uint value);
	void decrease_child_timers(uint value);

	int iterateChilds(struct node_data **node);
	int generate_child_address(struct net_address *address);
	void updateChildCounter(const struct net_address *node);
	void updateParentCounter(const struct net_address *node);

private:
	struct node_data *findChild(const struct net_address *child);

	static int getNewChildAddress(const struct net_address *parent,
	                              struct net_address *child,
	                              const int childId);
};

} /* namespace mesh */
