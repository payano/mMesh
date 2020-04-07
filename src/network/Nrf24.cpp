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
namespace network {

Nrf24::Nrf24(gpio::GPIOInterface *gpio, syscalls::SyscallsInterface *syscall)
{
	RF24 olle(gpio, syscall);
	olle.begin();
}

Nrf24::~Nrf24()
{
}

int Nrf24::init()
{
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
	(void)dest;
	(void)msg;
	return 0;
}

void Nrf24::recv_from(union mesh_internal_msg *msg)
{
	(void)msg;
}

}

#endif
