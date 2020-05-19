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
#include "SyscallsInterface.h"

namespace syscalls {


class SyscallsTest : public ::testing::Test {

	virtual void SetUp() override {
		std::cout << __FUNCTION__ << std::endl;
	}

	virtual void TearDown() override {
		std::cout << __FUNCTION__ << std::endl;
	}
};

TEST_F(SyscallsTest,TestCopyData){
	/* Static method in SyscallsInterface */
	uint32_t original[3] = {0xdeadbeef,0xdadababa,0xc0dec0de};
	uint32_t modified[3] = {0,0,0};

	SyscallsInterface::copy_data(&modified[0],&original[0], 1);
	ASSERT_EQ(0xef, (uint8_t)modified[0]);

	SyscallsInterface::copy_data(&modified[0],&original[0], 2);
	ASSERT_EQ(0xbeef, (uint16_t)modified[0]);

	SyscallsInterface::copy_data(&modified[0],&original[0], 4);
	ASSERT_EQ(0xdeadbeef, (uint32_t)modified[0]);

	SyscallsInterface::copy_data(&modified[0],&original[0], 8);
	ASSERT_EQ(0xbaba, (uint16_t)modified[1]);

	SyscallsInterface::copy_data(&modified[0],&original[0], 12);
	ASSERT_EQ(0xc0dec0de, (uint32_t)modified[2]);

}

TEST_F(SyscallsTest,TestCompareData){
	/* Static method in SyscallsInterface */
	uint32_t original[3] = {0xdeadbeef,0xdadababa,0xc0dec0de};
	uint32_t modified[3] = {0xdeadbeef,0xdadababa,0xc0dec0de};
	uint32_t wrong[3]    = {0xdadababa,0xc0dec0de, 0xdeadbeef};

	int ret;

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 1);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 2);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 3);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 4);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 5);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 6);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 7);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 8);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 9);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 10);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 11);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&modified[0],&original[0], 12);
	ASSERT_FALSE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 1);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 2);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 3);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 4);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 5);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 6);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 7);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 8);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 9);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 10);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 11);
	ASSERT_TRUE(ret);

	ret = SyscallsInterface::cmp_data(&wrong[0],&original[0], 12);
	ASSERT_TRUE(ret);

}

TEST_F(SyscallsTest,TestMemClear){
	/* Static method in SyscallsInterface */
	uint32_t original[3] = {0xdeadbeef,0xdadababa,0xc0dec0de};

	SyscallsInterface::mem_clr(original, 1);
	ASSERT_EQ(0x0, (uint8_t)original[0]);

	SyscallsInterface::mem_clr(original, 2);
	ASSERT_EQ(0x0, (uint16_t)original[0]);

	SyscallsInterface::mem_clr(original, 4);
	ASSERT_EQ((uint32_t)0x0, (uint32_t)original[0]);

	SyscallsInterface::mem_clr(original, 5);
	ASSERT_EQ(0x0, (uint8_t)original[1]);

	SyscallsInterface::mem_clr(original, 8);
	ASSERT_EQ((uint32_t)0x0, (uint32_t)original[1]);

	SyscallsInterface::mem_clr(original, 12);
	ASSERT_EQ((uint32_t)0x0, (uint32_t)original[2]);

}

}
