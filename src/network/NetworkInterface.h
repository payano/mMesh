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

#include <stdint.h>
#include "NetworkInterfacePublisher.h"
#include "DataTypes.h"
namespace network {

/*! \brief Base class for Network Interfaces.
 *
 *  The network interface is a base class that all
 *  for all network interfaces.
 */
class NetworkInterface : public NetworkInterfacePublisher{
	struct net_address mac;
public:
	virtual int init() = 0;
	virtual void deinit() = 0;
	virtual int start() = 0;
	virtual int sendto(const struct net_address *dest, union mesh_internal_msg *msg) = 0;
	virtual void recv_from(union mesh_internal_msg *msg) = 0;
	void setAddr(const struct net_address *addr) {NetHelper::copy_net_address(&mac, addr);}
	void getAddr(struct net_address *addr) {NetHelper::copy_net_address(addr, &mac);}
	virtual ~NetworkInterface(){}
};

} /* namespace network */

