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

struct networkData;
struct net_address;
union mesh_internal_msg;

namespace mesh {
class NetworkData;
}
namespace NetAlgorithm {

class NetAlgorithmInterface {
public:
	virtual const struct net_address *getRouteForPacket(const mesh::NetworkData *nw,
	                                       const struct net_address *to) = 0;
	/* if many nodes are responding, take the best of the alternatives. */
	virtual int associate_rsp_add_parent_to_list(mesh::NetworkData *nw,
	                                             union mesh_internal_msg *msg) = 0;
	virtual int choose_parent_from_list(mesh::NetworkData *nw,
	                                    struct net_address *parent) = 0;
	virtual int is_child_of(const struct net_address *parent,
	                      const struct net_address *child, bool include_master = true) = 0;
	virtual int evaluate_nb_address(mesh::NetworkData *nw,
	                                const struct net_address *nb_address) = 0;
	virtual void set_minimum_rank(int rank) = 0;
	virtual ~NetAlgorithmInterface(){}
};

} /* namespace routing */

