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
#ifndef UNIX

#include "Nrf24.h"
#include "RF24.h"
#include "DebugSingleton.h"
#include "DebugInterface.h"
#include "NetworkInterfaceSubscriber.h"
#include "STM32Syscalls.h"

namespace network {

Nrf24::Nrf24(syscalls::SyscallsInterface *syscall)
{
	this->syscall = syscall;
	debugger = debugger::DebugSingleton::getInstance();
	rf24 = new RF24(syscall);
	while(!rf24->isChipConnected())
	{
		syscall->msleep(10);
	}
	isConnected = rf24->isChipConnected();
	rf24->startListening();

	/* set broadcast address*/
	const void *v_bc_addr_ptr = static_cast<const void*>(&BROADCAST);
	const unsigned char *bc_addr_ptr = static_cast<const unsigned char *>(v_bc_addr_ptr);
	rf24->openReadingPipe(0,static_cast<const unsigned char *>(bc_addr_ptr));
}

Nrf24::~Nrf24()
{
}

int Nrf24::init()
{
	isConnected = rf24->isChipConnected();

	return 0;
}

void Nrf24::setSPI(void *spi)
{

}

void Nrf24::deinit()
{

}

int Nrf24::start()
{
	return 0;
}
int Nrf24::sendto(const struct net_address *dest, union mesh_internal_msg *msg)
{
	debugger->debug("Func: %s, Line: %d", __FILE__, __LINE__);
	rf24->stopListening();
	int ret;

	const void *v_addr_ptr = static_cast<const void*>(dest);
	const unsigned char *addr_ptr = static_cast<const unsigned char *>(v_addr_ptr);
	rf24->openWritingPipe(addr_ptr);

	/* Check the length of the message with the header no, for now send the union size*/
	ret = rf24->write(msg, sizeof(*msg));

	rf24->startListening();
	return ret == sizeof(*msg);
}

void Nrf24::recv_from(uint8_t from, union mesh_internal_msg *msg)
{
	debugger->debug("Func: %s, Line: %d", __FUNCTION__, __LINE__);
	cb->network_recv(msg);
}


void Nrf24::setAddr(const struct net_address *addr)
{
	NetworkInterface::setAddr(addr);
	if(1 == addr->gen_addr) {
		// only set data pipe 1
		const void *v_addr_ptr = static_cast<const void*>(addr);
		const unsigned char *addr_ptr = static_cast<const unsigned char *>(v_addr_ptr);
		rf24->openReadingPipe(0,static_cast<const unsigned char *>(addr_ptr));
		return;
	}


	struct net_address new_addresses;
	syscall->copy_data(&new_addresses, addr, sizeof(new_addresses));

	new_addresses.nbs[8].net = 0;
	void *v_addr_ptr = static_cast<void*>(&new_addresses);
	unsigned char *addr_ptr = static_cast<unsigned char *>(v_addr_ptr);
	for(int i = 1 ; i < 6 ; i++) {
		rf24->openReadingPipe(i,static_cast<unsigned char *>(addr_ptr));
		new_addresses.nbs[8].net++;
	}
	debugger->debug("Func: %s, Line: %d", __FUNCTION__, __LINE__);

}

void Nrf24::irq()
{
	/*
    * @param[out] tx_ok The send was successful (TX_DS)
    * @param[out] tx_fail The send failed, too many retries (MAX_RT)
    * @param[out] rx_ready There is a message waiting to be read (RX_DS)
    */
	debugger->debug("%s, %s, %d", __FILE__, __FUNCTION__, __LINE__);

	bool tx_ok, tx_fail, rx_ready;
	rf24->whatHappened(tx_ok, tx_fail, rx_ready);

	if(tx_ok) {
		// Do a signal?
	}

	if(tx_fail) {
		// Do a signal?
	}

	if(rx_ready) {
		uint8_t pipe_num; /* this we need later... */
		union mesh_internal_msg msg;
		rf24->available(&pipe_num);
		rf24->read(&msg, sizeof(msg));
		recv_from(pipe_num, &msg);
	}
}

}

#endif
