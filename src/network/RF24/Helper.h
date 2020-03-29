/*
 * Helper.h
 *
 *  Created on: Mar 27, 2020
 *      Author: evsejho
 */

#pragma once


#define _BV(x) (1<<(x))

namespace GPIO {
enum OUTPUTS {
	OUTPUT_LOW,
	OUTPUT_HIGH,
};
enum DIRECTION {
DIRECTION_IN,
DIRECTION_OUT,
};

static int write(uint16_t pin, uint16_t value){
		(void)pin;
		(void)value;
		return 0;
	}

static int open(uint16_t pin, bool value){
		(void)pin;
		(void)value;
		return 0;
	}

}

//#define __usleep(usec) usleep(usec)

class SPI {
	static void begin(){

	}
};
