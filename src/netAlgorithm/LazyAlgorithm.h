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

namespace NetAlgorithm {
class LazyAlgorithm : public NetAlgorithmInterface{
public:
	LazyAlgorithm();
	virtual ~LazyAlgorithm();

	const struct net_address *getRouteForPacket(const struct networkData *nw,
	                                            const struct net_address *to) override;
	/* if many nodes are responding, take the best of the alternatives. */
	int associate_rsp_add_parent_to_list(struct networkData *nw,
	                                     union mesh_internal_msg *msg) override;
	int choose_parent_from_list(struct networkData *nw,
	                                  struct net_address *parent) override;
};

} /* namespace NetAlgorithm */

