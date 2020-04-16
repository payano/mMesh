/*
 * STM32Syscalls.cpp
 *
 *  Created on: Apr 6, 2020
 *      Author: evsejho
 */
#ifndef UNIX

#include "STM32Syscalls.h"

namespace syscalls {

STM32Syscalls::STM32Syscalls(TIM_HandleTypeDef *htim1) : cpu_speed(0), per(0), psc(0), htim1(htim1) {
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
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(htim1, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_OnePulse_Init(htim1, TIM_OPMODE_SINGLE) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
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
	delay *= 0.001;
	psc = cpu_speed*delay/(per+1) - 1;
	set_htim_parameters();
	HAL_TIM_Base_Start(htim1);
	return 0;
}

bool STM32Syscalls::timer_started() {
//	if(false == firstRun) {
//		return false;
//	}
//	return htim1->Instance->CNT != 0 ? true : false;
	return true;
}

} /* namespace syscalls */

#endif
