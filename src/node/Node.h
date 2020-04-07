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

/* A node is a "stm32" controller with radio.
 * This is just for testing purposes
 */

#pragma once

#include "Mesh.h"
#include "Islands.h"
#include "DataTypes.h"
#include "DummyNWInterface.h"
#include <stdio.h>
#include <thread>         // std::thread

namespace network {
class NetworkInterface;
}

namespace syscalls {
class SyscallsInterface;
}

namespace node {

class Node {
private:
	friend class island::Island;
	mesh::Mesh *mesh;
	network::NetworkInterface *nw;
	syscalls::SyscallsInterface *syscalls;
	static int nodeId;
	std::thread *mThread;
	bool mThreadRunning;
	bool paused;
	void setAddr();
	void threadMain();
public:
	Node(network::NetworkInterface *nw);

	void getAddr(struct net_address *address);

	void addIsland(island::Island *island);
	void removeIsland(island::Island *island);
	void mute();
	void unmute();
	virtual ~Node();

	void pause(){paused = true;}
	void unpause(){paused = false;}
	void getParentAddress(struct net_address *addr) {mesh->getParent(addr);}
	bool getPaired(void) { return mesh->getPaired(); }
	bool getRegisteredToMaster(void) { return mesh->getRegisteredToMaster(); }
	int getNeighbourCount(void) { return mesh->getNeighbourCount(); }
	char *getName(void) { return mesh->getName();}

	void setMaster();

	void startThread();
	void stopThread();

	bool checkMaster(){ return mesh->getPaired() ;}

};

} /* namespace node */

