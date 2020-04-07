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
#include "NetworkInterfaceSubscriber.h"
#include "NetworkInterface.h"
#include "Islands.h"
#include <list>
namespace network {

/* Dummy interface should have islands.h */
class DummyNWInterface : public NetworkInterface {
private:
	std::list<island::Island *> islands;
	bool muted;

public:
	DummyNWInterface() : muted(false){}
	virtual ~DummyNWInterface(){}
	void setSPI(void *spi) override {
		(void)spi;
	}

	void addIsland(island::Island *island){islands.push_back(island);}
	void removeIsland(island::Island *island){islands.remove(island);}
	int init() override {return 0;}
	void deinit() override {}
	int start() override {return 0;}

	void mute() {
		muted = true;
	}
	void unmute(){
		muted = false;
	}
	int sendto(const struct net_address *dest, union mesh_internal_msg *msg) override {
		if(muted) return 0;
		for(island::Island *member : islands) {
			member->sendMessage(dest, msg);
		}
		return 0;
	}
	void recv_from(union mesh_internal_msg *msg) override {
		if(muted) return;
		if(nullptr == cb) return;
		cb->network_recv(msg);
	}
};

} /* namespace network */

