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


#include "Node.h"
#include <string.h>
#include <stdio.h> // REMOVE
#include <unistd.h>
#include <chrono>
namespace node {

int Node::nodeId = 0;

Node::Node(network::NetworkInterface *nw) {
	this->nw = nw;
	mThread = nullptr;
	mThreadRunning = false;
	// Might want to use threads to really simulate a "stm32" or "Arduino" controller.
	mesh = new mesh::Mesh(nw);
	paused = false;
	char name[10] = {0,};
	memset(name, '\0', 10);
	snprintf(name, 10, "Node%d",this->nodeId);
	mesh->setName(name);
//	printf("created node, NODEID: %d\n", nodeId);
	nodeId++;
	setAddr();
}

void Node::setAddr(){
	// Need to set a random number:
	/* initialize random seed: */

	struct net_address mac;
	NetHelper::generate_temporary_address(&mac);
//	printf("generated address\n");
//	NetHelper::printf_address(&mac);
	mesh->setTemporaryMacAddr(&mac);

}

void Node::getAddr(struct net_address *address) {
	nw->getAddr(address);
}

void Node::addIsland(island::Island *island)
{
	// Ugly hack
	static_cast<network::DummyNWInterface*>(nw)->addIsland(island);
}

void Node::removeIsland(island::Island *island)
{
	// Ugly hack
	static_cast<network::DummyNWInterface*>(nw)->removeIsland(island);
}

void Node::mute()
{
	static_cast<network::DummyNWInterface*>(nw)->mute();
}
void Node::unmute()
{
	static_cast<network::DummyNWInterface*>(nw)->unmute();

}

Node::~Node() {
	if(mThread != nullptr) stopThread();
	if(nullptr != mesh) delete mesh;
	if(nullptr != nw) delete nw;
	nodeId--;

}

void Node::setMaster(){
	mesh->setMaster();
}

void Node::threadMain()
{
	while(mThreadRunning) {
		if(false == paused) mesh->run();
		std::this_thread::sleep_for(std::chrono::milliseconds(RETRIGGER_TIMER));
	}
}

void Node::startThread(){
	mThreadRunning = true;
	mThread = new std::thread(&Node::threadMain, this);
}

void Node::stopThread(){
	if(mThread) {
		mThreadRunning = false;
		mThread->join();
		delete mThread;
		mThread = nullptr;
	}
}

} /* namespace node */
