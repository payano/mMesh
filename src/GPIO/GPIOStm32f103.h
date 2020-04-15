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
#pragma once
#include "GPIOInterface.h"

//class GPIO_TypeDef;

namespace gpio {

class GPIOStm32f103 : public GPIOInterface {
private:
	struct gpio_pins *pins;
public:
	GPIOStm32f103();
	virtual ~GPIOStm32f103();

	int init() override;
	void deinit() override;
	bool isset(PINS pin) override;
	void init_pins(struct gpio_pins *pins) override;
	void set_pin(PINS pin, bool level) override;

private:
	void read_pin(PINS *pin, GPIO_TypeDef **gpio_port, uint16_t *gpio_pin);
};

} /* namespace gpio */
#endif
