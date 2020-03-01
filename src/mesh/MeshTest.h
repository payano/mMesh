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
 * MeshTest.h
 *
 *  Created on: Feb 13, 2020
 *      Author: evsejho
 */

#ifndef SRC_MESHTEST_H_
#define SRC_MESHTEST_H_
#include "gtest/gtest.h"
#include "Mesh.h"
#include "../network/Nrf24.h"
#include "../network/NetworkInterface.h"

namespace mesh {

class MeshTest : public ::testing::Test {
protected:
	Mesh *mesh1;
	Mesh *mesh2;

	virtual void SetUp() override {
//		mesh1 = new Mesh(new Nrf24());
//		mesh2 = new Mesh(new Nrf24());
	}

	virtual void TearDown() override {
//		delete mesh1;
//		delete mesh2;
	}
};

TEST_F(MeshTest,Start){
	  ASSERT_NE(mesh1,mesh2);
}

} /* namespace mesh */


#endif /* SRC_MESHTEST_H_ */
