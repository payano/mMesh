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

/*
 * NetworkInterfacePublisher.h
 *
 *  Created on: Feb 19, 2020
 *      Author: evsejho
 */

#ifndef SRC_NETWORK_NETWORKINTERFACEPUBLISHER_H_
#define SRC_NETWORK_NETWORKINTERFACEPUBLISHER_H_

#include <stdio.h> // shall be removed later.

namespace network {
class NetworkInterfaceSubscriber;

class NetworkInterfacePublisher {
protected:
	NetworkInterfaceSubscriber *cb; /*!< When the network interface
	receives a message it will call the callback function  */
//	virtual void msg_received(NetworkInterface *nw, void *buf, uint16_t len) = 0;
public:
	void registerSubscriber(NetworkInterfaceSubscriber *cb){
		if(this->cb != nullptr) {
			printf("ERROR!\n");
		}
		this->cb = cb;
	}
	void ungisterSubscriber(){cb = nullptr;}
	NetworkInterfacePublisher() : cb(nullptr) {}
	virtual ~NetworkInterfacePublisher(){}
};

} /* namespace network */

#endif /* SRC_NETWORK_NETWORKINTERFACEPUBLISHER_H_ */
