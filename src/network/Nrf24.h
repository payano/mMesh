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
#include "NetworkInterface.h"

namespace syscalls {
class SyscallsInterface;
}
namespace gpio {
class GPIOInterface;
}

namespace spi {
class SPIInterface;
}
namespace network {
class RF24;
}

namespace debugger {
class DebugInterface;
class DebugSingleton;
}

namespace network {

class Nrf24 : public NetworkInterface {
private:
	bool isConnected;
	RF24 *rf24;
	debugger::DebugInterface *debugger;
	syscalls::SyscallsInterface *syscall;
protected:
public:
	Nrf24(syscalls::SyscallsInterface *syscall);
	virtual ~Nrf24();
	void setSPI(void *spi) override;
	int init() override;
	void deinit() override;
	int start() override;
	int sendto(const struct net_address *dest, union mesh_internal_msg *msg) override;
	void recv_from(uint8_t from, union mesh_internal_msg *msg) override;
	void setAddr(const struct net_address *addr) override;
	void irq() override;


};

} /* namespace network */

