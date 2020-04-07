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

#include "GPIOStm32f103.h"
#include "main.h"

namespace gpio {
GPIOStm32f103::GPIOStm32f103() : pins(nullptr){
	// TODO Auto-generated constructor stub

}

GPIOStm32f103::~GPIOStm32f103() {
	// TODO Auto-generated destructor stub
}

int GPIOStm32f103::init() {
	return 0;
};
void GPIOStm32f103::deinit() {
};
bool GPIOStm32f103::isset(PINS pin) {
	GPIO_TypeDef *gpio_port = nullptr;
	uint16_t gpio_pin = 0;

	read_pin(&pin, gpio_port, &gpio_pin);
	GPIO_PinState state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);
	return GPIO_PIN_SET == state ? true : false;
};

void GPIOStm32f103::init_pins(struct gpio_pins *pins) {
	this->pins = pins;
}

void GPIOStm32f103::set_pin(PINS pin, bool level) {
		GPIO_TypeDef *gpio_port = nullptr;
		uint16_t gpio_pin = 0;

		read_pin(&pin, gpio_port, &gpio_pin);
		GPIO_PinState pin_state = level == true ? GPIO_PIN_SET : GPIO_PIN_RESET;
		HAL_GPIO_WritePin(gpio_port, gpio_pin, pin_state);
};

/* circular problems... */
void GPIOStm32f103::read_pin(PINS *pin, void *gpio_port, uint16_t *gpio_pin) {
	switch(*pin){
	case PINS::CE_PIN:
		gpio_port = static_cast<GPIO_TypeDef *>(pins->ce_port);
		*gpio_pin = pins->ce_pin;
		break;
	case PINS::CSN_PIN:
		gpio_port = static_cast<GPIO_TypeDef *>(pins->csn_port);
		*gpio_pin = pins->csn_pin;
		break;
	/* don't create a default, or throw an exception in default. */
	}
}

} /* namespace gpio */
#endif
