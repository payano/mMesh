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
#include <inttypes.h>

namespace syscalls {
#define ARCH64 8
#define ARCH32 4
#define ARCH16 2
#define ARCH8  1

enum SPEED {
	SPEED_UNSET,
	SPEED_72MHZ,
};

struct gpio_pins{
//	GPIO_TypeDef *ce_port;
	void *ce_port;
	uint16_t ce_pin;
//	GPIO_TypeDef *csn_port;
	void *csn_port;
	uint16_t csn_pin;
};

enum PINS {
CE_PIN,
CSN_PIN,
};

class SyscallsInterface {
public:
	virtual ~SyscallsInterface(){}
	virtual void init() = 0;
	virtual void set_cpu_speed(SPEED speed) = 0;
	virtual void microsleep(int delay) = 0;
	virtual void msleep(int delay) = 0;
	virtual int start_timer(int delay) = 0;
	virtual bool timer_started() = 0;
	virtual int get_random() = 0;

	/* gpio */
	virtual bool gpio_isset(PINS pin) = 0;
	virtual void gpio_init_pins(struct gpio_pins *pins) = 0;
	virtual void gpio_set_pin(PINS pin, bool level) = 0;

	/* spi */
    virtual void spi_begin() = 0;
    virtual uint8_t spi_transfer(uint8_t tx) = 0;
    virtual void spi_transfernb(uint8_t *tbuf, uint8_t *rbuf, uint16_t len) = 0;
    virtual void spi_transfern(uint8_t *buf, uint16_t len) = 0;

    /* uart */
    virtual void uart_transmit(uint8_t *buffer, int len) = 0;


	static void copy_data(void *to, const void *from, int sz);
	static int cmp_data(const void *first, const void *second, int sz);
	static void mem_clr(void *buffer, int sz);
	static int string_len(const void *string_in);

private:
	void read_pin(PINS *pin, void **gpio_port, uint16_t *gpio_pin);

	static int copy_data8(void *to, const void *from, int *sz);
	static int copy_data4(void *to, const void *from, int *sz);
	static int copy_data2(void *to, const void *from, int *sz);
	static int copy_data1(void *to, const void *from, int *sz);
	static int cmp_data8(const void *first, const void *second, int *sz, int *compared);
	static int cmp_data4(const void *first, const void *second, int *sz, int *compared);
	static int cmp_data2(const void *first, const void *second, int *sz, int *compared);
	static int cmp_data1(const void *first, const void *second, int *sz, int *compared);
	static int mem_clr8(void *buffer, int *sz);
	static int mem_clr4(void *buffer, int *sz);
	static int mem_clr2(void *buffer, int *sz);
	static int mem_clr1(void *buffer, int *sz);

};

}
