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

#ifdef UNIX

#pragma once

#include "SPIInterface.h"

namespace spi {

class SPILinux : public SPIInterface {
public:
	SPILinux();
	virtual ~SPILinux();

	void setSPI(void *spidrv) override {
		(void) spidrv;
	}
	void begin() override {}
	uint8_t transfer(uint8_t tx) override {
		(void)tx;
		return 0;
	}
	void transfernb(uint8_t *tbuf, uint8_t *rbuf, uint16_t len) override {
		(void)tbuf;
		(void)rbuf;
		(void)len;
	}
	void transfern(uint8_t *buf, uint16_t len) override {
		(void)buf;
		(void)len;
	}

};

} /* namespace spi */

#endif
