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

#ifndef UNIX
#include "SPIStm32f103.h"
#include "main.h"
namespace spi {

SPIStm32f103::SPIStm32f103(SPI_HandleTypeDef *hspi) : hspi(hspi)
{
	// TODO Auto-generated constructor stub

}

SPIStm32f103::~SPIStm32f103() {
	// TODO Auto-generated destructor stub
}

void SPIStm32f103::setSPI(void *spidrv)
{
	(void)spidrv;
}


void SPIStm32f103::begin()
{

}

/**
* Transfer a single byte
* @param tx Byte to send
* @return Data returned via spi
*/
uint8_t SPIStm32f103::transfer(uint8_t tx) {
	uint8_t ret;
	SPI_HandleTypeDef *spi = hspi;
	HAL_SPI_TransmitReceive(spi, &tx, &ret, sizeof(uint8_t), 100);
	return ret;
}

/**
* Transfer a buffer of data
* @param tbuf Transmit buffer
* @param rbuf Receive buffer
* @param len Length of the data
*/
void SPIStm32f103::transfernb(uint8_t *tbuf, uint8_t *rbuf, uint16_t len) {
	SPI_HandleTypeDef *spi = hspi;
	HAL_SPI_TransmitReceive(spi, tbuf, rbuf, len, 100);
}

/**
* Transfer a buffer of data without an rx buffer
* @param buf Pointer to a buffer of data
* @param len Length of the data
*/
void SPIStm32f103::transfern(uint8_t *buf, uint16_t len) {
	SPI_HandleTypeDef *spi = hspi;
	HAL_SPI_Transmit(spi, buf, len, 100);
}

} /* namespace spi */

#endif
