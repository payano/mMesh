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
#include "DummyNWInterface.h"
#include "Islands.h"
#include "Node.h"
#include <chrono>

constexpr int SLAVE_TO_MASTER(5);
constexpr int SLAVE_COUNT(1);
constexpr int ISLANDS(10);
constexpr int STARTUP_TIME(1000000);
constexpr int STARTUP_TIME_MS(1000);

namespace node {

class NodeTest : public ::testing::Test {
public:
protected:
	Node *master;
	Node *slave_to_master[SLAVE_TO_MASTER];
	Node *slave_to_0[SLAVE_COUNT];
	Node *slave_to_0_0[SLAVE_COUNT];
	Node *slave_to_0_0_0[SLAVE_COUNT];
	Node *slave_to_0_0_0_0[SLAVE_COUNT];
	Node *slave_to_0_0_0_0_0[SLAVE_COUNT];
	Node *slave_to_0_0_0_0_0_0[SLAVE_COUNT];
	Node *slave_to_0_0_0_0_0_0_0[SLAVE_COUNT];
	Node *slave_to_0_0_0_0_0_0_0_0[SLAVE_COUNT];
	Node *slave_to_0_0_0_0_0_0_0_0_0[SLAVE_COUNT];
	island::Island islands[ISLANDS];

	virtual void SetUp() override {
		int slave_no = 0;
		std::cout << __FUNCTION__ << std::endl;
		master = new Node(new network::DummyNWInterface());

		master->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(master);
		for(int i = 0 ; i < SLAVE_TO_MASTER ; ++i){
			slave_to_master[i] = new Node(new network::DummyNWInterface());
			slave_to_master[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_master[i]);
		}
		slave_no++;

		slave_to_master[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_master[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0[i]);
		}
		slave_no++;

		slave_to_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0[i]);
		}
		slave_no++;

		slave_to_0_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0_0[i]);
		}
		slave_no++;

		slave_to_0_0_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0_0_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0_0_0[i]);
		}
		slave_no++;

		slave_to_0_0_0_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0_0_0_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0_0_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0_0_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0_0_0_0[i]);
		}
		slave_no++;

		slave_to_0_0_0_0_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0_0_0_0_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0_0_0_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0_0_0_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0_0_0_0_0[i]);
		}
		slave_no++;

		slave_to_0_0_0_0_0_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0_0_0_0_0_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0_0_0_0_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0_0_0_0_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0_0_0_0_0_0[i]);
		}
		slave_no++;

		slave_to_0_0_0_0_0_0_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0_0_0_0_0_0_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0_0_0_0_0_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0_0_0_0_0_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0_0_0_0_0_0_0[i]);
		}
		slave_no++;

		slave_to_0_0_0_0_0_0_0_0[0]->addIsland(&islands[slave_no]);
		islands[slave_no].registerToIsland(slave_to_0_0_0_0_0_0_0_0[0]);
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			slave_to_0_0_0_0_0_0_0_0_0[i] = new Node(new network::DummyNWInterface());
			slave_to_0_0_0_0_0_0_0_0_0[i]->addIsland(&islands[slave_no]);
			islands[slave_no].registerToIsland(slave_to_0_0_0_0_0_0_0_0_0[i]);
		}
		slave_no++;

	}

	virtual void TearDown() override {
		std::cout << __FUNCTION__ << std::endl;
		delete master;
		for(int i = 0 ; i < SLAVE_TO_MASTER ; ++i){delete slave_to_master[i];}
		for(int i = 0 ; i < SLAVE_COUNT ; ++i){
			delete slave_to_0[i];
			delete slave_to_0_0[i];
			delete slave_to_0_0_0[i];
			delete slave_to_0_0_0_0[i];
			delete slave_to_0_0_0_0_0[i];
			delete slave_to_0_0_0_0_0_0[i];
			delete slave_to_0_0_0_0_0_0_0[i];
			delete slave_to_0_0_0_0_0_0_0_0[i];
			delete slave_to_0_0_0_0_0_0_0_0_0[i];
		}

		for(int i = 0 ; i < ISLANDS ; ++i) { islands[i].reset();}
	}

}; /* namespace node */


TEST_F(NodeTest,StartNoAssignments){
	struct net_address addr;
	std::cout << "TESTING StartNoAssignments" << std::endl;

	std::cout << "Printing results:" << std::endl;
	std::cout << "Master: ";
	master->getAddr(&addr);
	NetHelper::printf_address(&addr);
	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {
		printf("%s : ", slave_to_master[i]->getName());
		slave_to_master[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);

//		 Check if connected to parent
		slave_to_master[i]->getParentAddress(&addr);
		ASSERT_FALSE(NetHelper::compare_net_address(&MASTER, &addr));
		ASSERT_FALSE(slave_to_master[i]->getPaired());
	}
}

TEST_F(NodeTest,Assign5Nodes2Master){
	struct net_address addr;
	std::cout << "TESTING Assign5Nodes2Master" << std::endl;
	std::cout << "Setting master" << std::endl;
	master->setMaster();
	master->startThread();

	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {slave_to_master[i]->startThread();}

	std::this_thread::sleep_for(std::chrono::milliseconds(3*STARTUP_TIME_MS));

	master->stopThread();

	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {
		ASSERT_TRUE(slave_to_master[i]->getRegisteredToMaster());
		slave_to_master[i]->stopThread();
	}

	master->getAddr(&addr);
	printf("Master: ");
	NetHelper::printf_address(&addr);
	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {
		slave_to_master[i]->getAddr(&addr);
		printf("%s : ", slave_to_master[i]->getName());
		NetHelper::printf_address(&addr);
	}
}

TEST_F(NodeTest,Associate2Islands){
	struct net_address addr;
	std::cout << "TESTING SlaveSendRegister2Master" << std::endl;
	std::cout << "Setting master" << std::endl;
	master->setMaster();
	master->startThread();


	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {slave_to_master[i]->startThread();}
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		slave_to_0[i]->startThread();
		slave_to_0_0[i]->startThread();
		slave_to_0_0_0[i]->startThread();
		slave_to_0_0_0_0[i]->startThread();
		slave_to_0_0_0_0_0[i]->startThread();
		slave_to_0_0_0_0_0_0[i]->startThread();
		slave_to_0_0_0_0_0_0_0[i]->startThread();
		slave_to_0_0_0_0_0_0_0_0[i]->startThread();
		slave_to_0_0_0_0_0_0_0_0_0[i]->startThread();
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(7*STARTUP_TIME_MS));

	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {slave_to_master[i]->stopThread();}
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		slave_to_0[i]->stopThread();
		slave_to_0_0[i]->stopThread();
		slave_to_0_0_0[i]->stopThread();
		slave_to_0_0_0_0[i]->stopThread();
		slave_to_0_0_0_0_0[i]->stopThread();
		slave_to_0_0_0_0_0_0[i]->stopThread();
		slave_to_0_0_0_0_0_0_0[i]->stopThread();
		slave_to_0_0_0_0_0_0_0_0[i]->stopThread();
		slave_to_0_0_0_0_0_0_0_0_0[i]->stopThread();
	}

	printf("\nIslands statistics (BC = Broadcast msg): \n");
	for(int i = 0; i < ISLANDS ; ++i){
		printf("Island[%d]\n", i);
		islands[i].printStatistics();
	}

	master->getAddr(&addr);
	printf("Master: ");
	NetHelper::printf_address(&addr);

	printf("---- Slaves to master ----\n");
	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {
		printf("%7s : ", slave_to_master[i]->getName());
		slave_to_master[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_master[i]->getPaired());
		ASSERT_TRUE(slave_to_master[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node1 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0[i]->getName());
		slave_to_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node6 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0_0[i]->getName());
		slave_to_0_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0_0[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node10 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0_0_0[i]->getName());
		slave_to_0_0_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0_0_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0_0_0[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node14 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0_0_0_0[i]->getName());
		slave_to_0_0_0_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0_0_0_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0_0_0_0[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node18 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0_0_0_0_0[i]->getName());
		slave_to_0_0_0_0_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0_0_0_0_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0_0_0_0_0[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node22 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0_0_0_0_0_0[i]->getName());
		slave_to_0_0_0_0_0_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0_0_0_0_0_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0_0_0_0_0_0[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node26 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0_0_0_0_0_0_0[i]->getName());
		slave_to_0_0_0_0_0_0_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0_0_0_0_0_0_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0_0_0_0_0_0_0[i]->getRegisteredToMaster());
	}

	printf("---- Slaves to Node30 ----\n");
	for(int i = 0 ; i < SLAVE_COUNT ; ++i){
		printf("%7s : ", slave_to_0_0_0_0_0_0_0_0[i]->getName());
		slave_to_0_0_0_0_0_0_0_0[i]->getAddr(&addr);
		NetHelper::printf_address(&addr);
		ASSERT_TRUE(slave_to_0_0_0_0_0_0_0_0[i]->getPaired());
		ASSERT_TRUE(slave_to_0_0_0_0_0_0_0_0[i]->getRegisteredToMaster());
	}

}

//TEST_F(NodeTest,SeekNBAndParents){
//	struct net_address addr;
//	std::cout << "TESTING SlaveSendRegister2Master" << std::endl;
//	std::cout << "Setting master" << std::endl;
//	master->setMaster();
//	master->startThread();
//
//	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {slave_to_master[i]->startThread();}
//
//	std::this_thread::sleep_for(std::chrono::milliseconds(3*STARTUP_TIME_MS));
//
//	for(int i = 0; i < SLAVE_TO_MASTER ; i++) {slave_to_master[i]->stopThread();}
//
//	printf("\nIslands statistics (BC = Broadcast msg): \n");
//	for(int i = 0; i < 1 ; ++i){
//		printf("Island[%d]\n", i);
//		islands[i].printStatistics();
//	}
//
//		printf("---- Slaves to master ----\n");
//		for(int i = 0; i < SLAVE_TO_MASTER ; i++) {
//			printf("%7s : ", slave_to_master[i]->getName());
//			slave_to_master[i]->getAddr(&addr);
//			NetHelper::printf_address(&addr);
//			ASSERT_TRUE(slave_to_master[i]->getPaired());
//			ASSERT_TRUE(slave_to_master[i]->getRegisteredToMaster());
//		}
//}

}
