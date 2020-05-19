/*
 * STM32Syscalls.cpp
 *
 *  Created on: Apr 6, 2020
 *      Author: evsejho
 */
#ifndef UNIX

#include "STM32Syscalls.h"
#include "stm32f1xx_hal.h"

namespace syscalls {

STM32Syscalls::STM32Syscalls(void *htim1, void *hadc1, void *hspi1, void *huart1)
: cpu_speed(0), per(0), psc(0), pins(nullptr), htim(htim1), hadc(hadc1), hspi(hspi1), huart(huart1)
{
	firstRun = false;
	// TODO Auto-generated constructor stub

}

STM32Syscalls::~STM32Syscalls() {
	// TODO Auto-generated destructor stub
}

void STM32Syscalls::init() {
	set_htim_parameters();
}


void STM32Syscalls::set_cpu_speed(SPEED speed) {
	/*
	 * The formula for getting PSC and ARR is:
	 * arr_multiplier = cpu_speed/72Mhz
	 * arr = arr_multiplier * 0xFFFF
	 */
	uint32_t rc_multiplier;
	switch(speed) {
	case SPEED_72MHZ:
		cpu_speed = 72000000;
		rc_multiplier = cpu_speed/72000000;
		this->per = 0xFFFF * rc_multiplier;
		break;
	default:
		/* this is error */
		rc_multiplier = 0;
	}

}

void STM32Syscalls::set_htim_parameters()
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	TIM_HandleTypeDef *htim1 = static_cast<TIM_HandleTypeDef*>(htim);

	htim1->Instance = TIM1;
//	htim1->Init.Prescaler = 10986;
	htim1->Init.Prescaler = psc;
	htim1->Init.CounterMode = TIM_COUNTERMODE_UP;
//	htim1->Init.Period = 0x0fff;
	htim1->Init.Period = per;
	htim1->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1->Init.RepetitionCounter = 0;
	htim1->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(htim1) != HAL_OK)
	{
//		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(htim1, &sClockSourceConfig) != HAL_OK)
	{
//		Error_Handler();
	}
	if (HAL_TIM_OnePulse_Init(htim1, TIM_OPMODE_SINGLE) != HAL_OK)
	{
//		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(htim1, &sMasterConfig) != HAL_OK)
	{
//		Error_Handler();
	}
}

void STM32Syscalls::microsleep(int delay) {
	(void)delay;
	HAL_Delay(1);

}
void STM32Syscalls::msleep(int delay) {
	HAL_Delay(delay);
}

int STM32Syscalls::start_timer(int delay) {
	/*
	 * Here we need to calculate PSC
	 *
	 * The formula is:
	 * T = ((PSC + 1)(ARR + 1))/CPU_SPEED
	 *
	 * T = delay [ms]
	 *
	 * ARR is known.
	 *
	 * Therefore:
	 * PSC = CPU_SPEED*T/(ARR+1) -1
	 *
	 * And T is [ms]
	 * PSC = CPU_SPEED*(T*10^-3)/(ARR+1) -1
	 */
//	psc = cpu_speed*delay/(per+1) - 1;
	TIM_HandleTypeDef *htim1 = static_cast<TIM_HandleTypeDef*>(htim);

	float fdelay = delay *0.001;
	psc = cpu_speed*fdelay/(per+1) - 1;
	set_htim_parameters();
	HAL_TIM_Base_Start(htim1);
	return 0;
}

bool STM32Syscalls::timer_started() {
//	if(false == firstRun) {
//		return false;
//	}
	TIM_HandleTypeDef *htim1 = static_cast<TIM_HandleTypeDef*>(htim);

	return htim1->Instance->CNT != 0 ? true : false;
}

int STM32Syscalls::get_random()
{
	ADC_HandleTypeDef *hadc1 = static_cast<ADC_HandleTypeDef *>(hadc);
	constexpr float V25 = 1.43;
	constexpr float average_slope = 4.3;

	int randomness, readval;
	HAL_ADC_Start(hadc1);
	HAL_ADC_PollForConversion(hadc1, 1000);
	readval = HAL_ADC_GetValue(hadc1);
	HAL_ADC_Stop(hadc1);

	randomness = ((V25 * readval) / average_slope) * 100000;
	return (uint32_t) randomness;

//	random_val = random_val << 32;
//	HAL_ADC_Start(hadc1);
//	HAL_ADC_PollForConversion(hadc1, 1000);
//	readval = HAL_ADC_GetValue(hadc1);
//	HAL_ADC_Stop(hadc1);
//	HAL_Delay(1000);
//
//	randomness = ((V25 * readval) / average_slope) * 100000;
//	random_val |= (uint32_t) randomness;
//	return 0;
}

bool STM32Syscalls::gpio_isset(PINS pin) {
	void *gpio_port_v;
	uint16_t gpio_pin = 0;

	gpio_read_pin(&pin, &gpio_port_v, &gpio_pin);
	GPIO_TypeDef *gpio_port = static_cast<GPIO_TypeDef *>(gpio_port_v);

	GPIO_PinState state = HAL_GPIO_ReadPin(gpio_port, gpio_pin);
	return GPIO_PIN_SET == state ? true : false;
};

void STM32Syscalls::gpio_init_pins(struct gpio_pins *pins) {
	this->pins = pins;
}

void STM32Syscalls::gpio_set_pin(PINS pin, bool level) {
	void *gpio_port_v;
	uint16_t gpio_pin = 0;
	GPIO_PinState pin_state = level == true ? GPIO_PIN_SET : GPIO_PIN_RESET;
	//
	gpio_read_pin(&pin, &gpio_port_v, &gpio_pin);
	GPIO_TypeDef *gpio_port = static_cast<GPIO_TypeDef *>(gpio_port_v);
	HAL_GPIO_WritePin(gpio_port, gpio_pin, pin_state);
//	gpio_port = RF24_CE_GPIO_Port;
//	gpio_pin = RF24_CE_Pin;
//	gpio_port = RF24_CSN_GPIO_Port;
//	gpio_pin = RF24_CSN_Pin;
	HAL_GPIO_WritePin(gpio_port, gpio_pin, pin_state);
};

/* circular problems... */
void STM32Syscalls::gpio_read_pin(PINS *pin, void **gpio_port, uint16_t *gpio_pin) {
	switch(*pin){
	case PINS::CE_PIN:
		*gpio_port = pins->ce_port;
		*gpio_pin = pins->ce_pin;
		break;
	case PINS::CSN_PIN:
		*gpio_port = pins->csn_port;
		*gpio_pin = pins->csn_pin;
		break;
	/* don't create a default, or throw an exception in default. */
	}
}

void STM32Syscalls::spi_begin()
{

}
/**
* Transfer a single byte
* @param tx Byte to send
* @return Data returned via spi
*/
uint8_t STM32Syscalls::spi_transfer(uint8_t tx) {
	uint8_t ret;
	SPI_HandleTypeDef *spi = static_cast<SPI_HandleTypeDef *>(hspi);
	HAL_SPI_TransmitReceive(spi, &tx, &ret, sizeof(uint8_t), 100);
	return ret;
}

/**
* Transfer a buffer of data
* @param tbuf Transmit buffer
* @param rbuf Receive buffer
* @param len Length of the data
*/
void STM32Syscalls::spi_transfernb(uint8_t *tbuf, uint8_t *rbuf, uint16_t len) {
	SPI_HandleTypeDef *spi = static_cast<SPI_HandleTypeDef *>(hspi);
	HAL_SPI_TransmitReceive(spi, tbuf, rbuf, len, 100);
}

/**
* Transfer a buffer of data without an rx buffer
* @param buf Pointer to a buffer of data
* @param len Length of the data
*/
void STM32Syscalls::spi_transfern(uint8_t *buf, uint16_t len) {
	SPI_HandleTypeDef *spi = static_cast<SPI_HandleTypeDef *>(hspi);
	HAL_SPI_Transmit(spi, buf, len, 100);
}

void STM32Syscalls::uart_transmit(uint8_t *buffer, int len) {
	HAL_UART_Transmit(static_cast<UART_HandleTypeDef*>(huart), (uint8_t*)buffer, len, 100);

}

} /* namespace syscalls */

#endif
