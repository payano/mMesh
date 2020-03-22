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
#include "NetAlgorithmInterface.h"

namespace mesh {
class NetworkData;
}
namespace NetAlgorithm {
class LazyAlgorithm : public NetAlgorithmInterface {
	int rank;
public:
	LazyAlgorithm();
	virtual ~LazyAlgorithm();

	const struct net_address *getRouteForPacket(const mesh::NetworkData *nw,
	                                            const struct net_address *to) override;
	/* if many nodes are responding, take the best of the alternatives. */
	int associate_rsp_add_parent_to_list(mesh::NetworkData *nw,
	                                     union mesh_internal_msg *msg) override;
	int choose_parent_from_list(mesh::NetworkData*nw,
	                                  struct net_address *parent) override;
	int is_child_of(const struct net_address *parent,
	                const struct net_address *child, bool include_master=true) override;
	int evaluate_nb_address(mesh::NetworkData *nw,
	                        const struct net_address *nb_address) override;
	void set_minimum_rank(int rank) override;
private:
	int get_address_depth(const struct net_address *nb_address);
	int get_common_ancestor(const struct net_address *mac,
	                        const struct net_address *nb_address);
};

} /* namespace NetAlgorithm */

