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

#include "gtest/gtest.h"
#include <chrono>
#include "LazyAlgorithm.h"
#include "NetAlgorithmInterface.h"
#include "NetworkData.h"
#include "DataTypes.h"
#include "LinuxSyscalls.h"

namespace NetAlgorithm {
const constexpr int testAddresses(5);

class LazyAlgorithmTest : public ::testing::Test {
public:
	NetAlgorithmInterface *algorithm;
	syscalls::SyscallsInterface *syscalls;
	mesh::NetworkData *networkdata;
	struct net_address testnodes[testAddresses];

	void SetUp() override {
		syscalls = new syscalls::LinuxSyscalls();
		networkdata = new mesh::NetworkData(syscalls);
		std::cout << __FUNCTION__ << std::endl;
		algorithm = new LazyAlgorithm();
		for(int i = 0; i < testAddresses; ++i) {
			mem_clr(&testnodes[i], sizeof(testnodes[i]));
		}
	}

	void TearDown() override {
		std::cout << __FUNCTION__ << std::endl;
		delete networkdata;
		delete syscalls;
		delete algorithm;
	}
};

TEST_F(LazyAlgorithmTest,TestNBs){
	int ret;
	networkdata->mac.nbs[0].net = 0x1; // Connected to master

	testnodes[0].nbs[0].net = 0x2;

	testnodes[1].nbs[0].net = 0x1;
	testnodes[1].nbs[1].net = 0x1;

	testnodes[2].nbs[0].net = 0x1;
	testnodes[2].nbs[1].net = 0x2;
	testnodes[2].nbs[2].net = 0x1;

	testnodes[3].nbs[0].net = 0x1;
	testnodes[3].nbs[1].net = 0x2;
	testnodes[3].nbs[2].net = 0x1;
	testnodes[3].nbs[3].net = 0x1;

	testnodes[4].master = 0x1;

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[0]);
	ASSERT_FALSE(ret);

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[1]);
	ASSERT_FALSE(ret);

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[2]);
	ASSERT_FALSE(ret);

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[3]);
	ASSERT_TRUE(ret);

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[4]);
	ASSERT_FALSE(ret);
}

TEST_F(LazyAlgorithmTest,TestRouting){
	int ret;
	networkdata->mac.nbs[0].net = 0x1;
	networkdata->mac.nbs[1].net = 0x1;
	networkdata->mac.nbs[2].net = 0x1;
	networkdata->registeredToMaster = 1;
	networkdata->mPaired = 1;

	networkdata->parent.connected = true;
	networkdata->parent.mac.nbs[0].net = 0x1;
	networkdata->parent.mac.nbs[1].net = 0x1;

	networkdata->pairedChildren = 1;
	networkdata->childs[0].connected = 1;
	networkdata->childs[0].mac.nbs[0].net = 0x1;
	networkdata->childs[0].mac.nbs[1].net = 0x1;
	networkdata->childs[0].mac.nbs[2].net = 0x1;
	networkdata->childs[0].mac.nbs[3].net = 0x4;

	testnodes[0].nbs[0].net = 0x5;
	testnodes[0].nbs[1].net = 0x4;
	testnodes[0].nbs[2].net = 0x3;
	testnodes[0].nbs[3].net = 0x2;
	testnodes[0].nbs[4].net = 0x1;

	testnodes[1].nbs[0].net = 0x2;
	testnodes[1].nbs[1].net = 0x3;
	testnodes[1].nbs[2].net = 0x3;
	testnodes[1].nbs[3].net = 0x2;
	testnodes[1].nbs[4].net = 0x1;
	testnodes[1].nbs[5].net = 0x3;
	testnodes[1].nbs[6].net = 0x2;

	testnodes[2].nbs[0].net = 0x5;
	testnodes[2].nbs[1].net = 0x4;
	testnodes[2].nbs[2].net = 0x4;
	testnodes[2].nbs[3].net = 0x4;
	testnodes[2].nbs[4].net = 0x4;
	testnodes[2].nbs[5].net = 0x4;
	testnodes[2].nbs[6].net = 0x4;

	testnodes[3].master = 0x1;

	// Add nodes to neighbors
	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[0]);
	ASSERT_TRUE(ret);

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[1]);
	ASSERT_TRUE(ret);

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[2]);
	ASSERT_TRUE(ret);

	ret = algorithm->evaluate_nb_address(networkdata, &testnodes[3]);
	ASSERT_TRUE(ret);

	// Addresses configured, try to test to routing.
	struct net_address dest_addr;
	const struct net_address *route_addr;
	mem_clr(&dest_addr, sizeof(dest_addr));

	// This shall go to testnodes[3]
	dest_addr.master = 0x1;
	route_addr = algorithm->getRouteForPacket(networkdata, &dest_addr);
	ret = cmp_data(route_addr, &testnodes[3],
	               sizeof(*route_addr));
	ASSERT_FALSE(ret);

	// This shall go to testnodes[0]
	mem_clr(&dest_addr, sizeof(dest_addr));
	dest_addr.nbs[0].net = 0x5;
	dest_addr.nbs[1].net = 0x4;
	dest_addr.nbs[2].net = 0x3;
	route_addr = algorithm->getRouteForPacket(networkdata, &dest_addr);
	ret = cmp_data(route_addr, &testnodes[0], sizeof(*route_addr));
	ASSERT_FALSE(ret);

	// This shall go to testnodes[1]
	mem_clr(&dest_addr, sizeof(dest_addr));
	dest_addr.nbs[0].net = 0x2;
	dest_addr.nbs[1].net = 0x3;
	dest_addr.nbs[2].net = 0x3;
	dest_addr.nbs[3].net = 0x2;
	dest_addr.nbs[4].net = 0x1;
	dest_addr.nbs[5].net = 0x3;
	dest_addr.nbs[6].net = 0x2;
	dest_addr.nbs[7].net = 0x2;
	route_addr = algorithm->getRouteForPacket(networkdata, &dest_addr);
	ret = cmp_data(route_addr, &testnodes[1], sizeof(*route_addr));
	ASSERT_FALSE(ret);

	// This shall go to testnodes[2]
	mem_clr(&dest_addr, sizeof(dest_addr));
	dest_addr.nbs[0].net = 0x5;
	dest_addr.nbs[1].net = 0x4;
	dest_addr.nbs[2].net = 0x4;
	dest_addr.nbs[3].net = 0x4;
	dest_addr.nbs[4].net = 0x4;
	dest_addr.nbs[5].net = 0x4;
	dest_addr.nbs[6].net = 0x4;
	route_addr = algorithm->getRouteForPacket(networkdata, &dest_addr);
	ret = cmp_data(route_addr, &testnodes[2], sizeof(*route_addr));
	ASSERT_FALSE(ret);

}

} /* namespace NetAlgorithm */

