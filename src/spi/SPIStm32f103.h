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
#include "SPIInterface.h"
#include "main.h"

namespace spi {
class SPIStm32f103 : public SPIInterface {
private:
	SPI_HandleTypeDef *hspi;
public:
	SPIStm32f103() : hspi(nullptr){}
	SPIStm32f103(SPI_HandleTypeDef *hspi);
	virtual ~SPIStm32f103();
	void setSPI(void *spidrv) override;
    void begin() override;
    /**
    * Transfer a single byte
    * @param tx Byte to send
    * @return Data returned via spi
    */
    uint8_t transfer(uint8_t tx) override;
        /**
    * Transfer a buffer of data
    * @param tbuf Transmit buffer
    * @param rbuf Receive buffer
    * @param len Length of the data
    */
    void transfernb(uint8_t *tbuf, uint8_t *rbuf, uint16_t len) override;

    /**
    * Transfer a buffer of data without an rx buffer
    * @param buf Pointer to a buffer of data
    * @param len Length of the data
    */
    void transfern(uint8_t *buf, uint16_t len) override ;
};

} /* namespace spi */

