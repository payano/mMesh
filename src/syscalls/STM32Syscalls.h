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
#ifndef UNIX

#include "SyscallsInterface.h"
namespace syscalls {

class STM32Syscalls : public SyscallsInterface {
private:
	uint32_t cpu_speed;
	uint16_t per;
	uint16_t psc;
	struct gpio_pins *pins;
	void *htim;
	void *hadc;
	void *hspi;
	void *huart;

	bool firstRun;
	void gpio_read_pin(PINS *pin, void **gpio_port, uint16_t *gpio_pin);

public:
	STM32Syscalls(void *htim1, void *hadc1, void *hspi1, void *huart1);
	virtual ~STM32Syscalls();
	void set_htim_parameters();

	void init() override;
	void set_cpu_speed(SPEED speed) override;
	void microsleep(int delay) override;
	void msleep(int delay) override;
	int start_timer(int delay) override;
	bool timer_started() override;
	int get_random() override;

	/* gpio */
	bool gpio_isset(PINS pin) override;
	void gpio_init_pins(struct gpio_pins *pins) override;
	void gpio_set_pin(PINS pin, bool level) override;

	/* spi */
    void spi_begin() override;
    uint8_t spi_transfer(uint8_t tx) override;
    void spi_transfernb(uint8_t *tbuf, uint8_t *rbuf, uint16_t len) override;
    void spi_transfern(uint8_t *buf, uint16_t len) override;

    /* uart */
    void uart_transmit(uint8_t *buffer, int len) override;


};

} /* namespace syscalls */

#endif
