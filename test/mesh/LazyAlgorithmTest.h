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

namespace NetAlgorithm {
const constexpr int testAddresses(5);

class LazyAlgorithmTest : public ::testing::Test {
public:
	NetAlgorithmInterface *algorithm;
	mesh::NetworkData networkdata;
	struct net_address testnodes[testAddresses];

	virtual void SetUp() override {
		std::cout << __FUNCTION__ << std::endl;
		algorithm = new LazyAlgorithm();
		for(int i = 0; i < testAddresses; ++i) {
			mem_clr(&testnodes[i], sizeof(testnodes[i]));
		}
	}

	virtual void TearDown() override {
		std::cout << __FUNCTION__ << std::endl;
		delete algorithm;
	}
};

TEST_F(LazyAlgorithmTest,TestNBs){
	int ret;
	networkdata.mac.nbs[0].net = 0x1; // Connected to master

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

	ret = algorithm->evaluate_nb_address(&networkdata, &testnodes[0]);
	ASSERT_FALSE(ret);

	ret = algorithm->evaluate_nb_address(&networkdata, &testnodes[1]);
	ASSERT_FALSE(ret);

	ret = algorithm->evaluate_nb_address(&networkdata, &testnodes[2]);
	ASSERT_FALSE(ret);

	ret = algorithm->evaluate_nb_address(&networkdata, &testnodes[3]);
	ASSERT_TRUE(ret);
}

} /* namespace NetAlgorithm */

