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


/* DEBUG LEVEL = DEBUG, ERROR, INFO */
#define DEBUG_LEVEL DEBUG

#ifndef UNIX
#include "main.h"
#endif

#include "DebugInterface.h"
#include "DataTypes.h"
#include <stdarg.h>
#include "SyscallsInterface.h"


namespace debugger {
enum DATA_TYPE {
	DATA_TYPE_UNDEFINED = 0,
	DATA_TYPE_STRING,
	DATA_TYPE_SIGNED_INT,
	DATA_TYPE_UNSIGNED_INT,
	DATA_TYPE_HEX,
};

class STM32Debug : public DebugInterface {
private:
#ifndef UNIX
	UART_HandleTypeDef *huart;
#endif
public:
#ifndef UNIX
	STM32Debug(void *uart){
		huart = static_cast<UART_HandleTypeDef*>(uart);
	}
#else
	STM32Debug(void *uart){
		(void)uart;
	}
#endif

private:
	int get_argument(const char **arg)
	{
		const char *val = (const char*)*arg;
		++val;
		enum DATA_TYPE type = DATA_TYPE_UNDEFINED;

		switch(*val){
		case 's':
			type =  DATA_TYPE_STRING;
			break;
		case 'd':
			type = DATA_TYPE_SIGNED_INT;
			break;
		case 'u':
			type = DATA_TYPE_UNSIGNED_INT;
			break;
		case 'x':
			type = DATA_TYPE_HEX;
			break;

		}

		++val;
		*arg = val;
		return type;
	}

	void add_int(char **buf_ptr, uint32_t value, int base, int signedval)
	{
		char *buffer = *buf_ptr;
		/* if zero just return zero */
		if(0 == value) {
			*buffer = '0';
			++buffer;
			return;
		}

		char rev_val[10];
		syscalls::SyscallsInterface::mem_clr(rev_val, 10);

		char *data_ptr = rev_val;
		int val;
		int count = 0;
		int negative = 0;

		if(signedval) {
			int32_t sval;
			syscalls::SyscallsInterface::copy_data((uint32_t*)&sval, &value, sizeof(value));
			if(sval < 0) {
				negative = 1;
				sval *= -1;
				value = sval;
			}
		}

		/* We need to know signedness if we want to express negative values */
		while(value > 0) {
			val = value % base;
			val += val < 10 ? 48 : 48+7 ; /* get ascii of the value.*/
			*data_ptr = val;
			value /= base;
			++data_ptr;
			++count;
		}

		if(negative) {
			*buffer = '-';
			++buffer;
		}

		// reverse the itoa
		char *int_iterator = &rev_val[count-1];
		while(int_iterator != rev_val) {
			*buffer = *int_iterator;
			--int_iterator;
			++buffer;
		}

		/* take last as well */
		*buffer = *int_iterator;
		++buffer;
		if(negative)
			++count;

		*buf_ptr = buffer;

	}

	void add_string(char **buffer, const char *string)
	{
		char *buf_ptr = *buffer;
		int len = syscalls::SyscallsInterface::string_len(string);

		syscalls::SyscallsInterface::copy_data(buf_ptr, string, len);
		*buffer = buf_ptr;
		buf_ptr += len;
		*buffer = buf_ptr;
	}

	void logger(char* buffer_ptr, const char *fmt, va_list *arguments)
	{
		int ret = 0;
		while(*fmt != 0){
			if(*fmt == '%') {
				ret = get_argument(&fmt);
				switch(ret) {
				case DATA_TYPE_STRING:
					add_string(&buffer_ptr, va_arg(*arguments,char *));
					continue;
					break;

				case DATA_TYPE_SIGNED_INT:
				case DATA_TYPE_UNSIGNED_INT:
					add_int(&buffer_ptr, va_arg(*arguments, unsigned int), 10, 1);
					continue;
					break;
				case DATA_TYPE_HEX:
					add_int(&buffer_ptr, va_arg(*arguments, unsigned int), 16, 0);
					continue;
					break;
				default:
					return;

				}
			}
			*buffer_ptr = *fmt;
			++buffer_ptr;
			++fmt;
		}
	}

public:

	void error(const char *fmt, ...) override {
		char buffer[100];
		char *buf_ptr = buffer;
		syscalls::SyscallsInterface::mem_clr(buffer, 100);
		const char *prefix = "ERROR: ";
		const char *suffix = "\n\r";
		int sz = syscalls::SyscallsInterface::string_len(prefix);
		syscalls::SyscallsInterface::copy_data(buf_ptr, prefix, sz);
		buf_ptr += sz;

		va_list args;
		va_start(args, fmt);
		logger(buf_ptr, fmt, &args);
		va_end(args);

		buf_ptr = buffer;
		sz = syscalls::SyscallsInterface::string_len(buffer);
		buf_ptr += sz;
		sz = syscalls::SyscallsInterface::string_len(suffix);
		syscalls::SyscallsInterface::copy_data(buf_ptr, suffix, sz);

		sz = syscalls::SyscallsInterface::string_len(buffer);
#ifndef UNIX
		HAL_UART_Transmit(huart, (uint8_t*)buffer, syscalls::SyscallsInterface::string_len(buffer), 100);
#endif

	}

	void warn(const char *fmt, ...) override {
		(void)fmt;
	}
	void info(const char *fmt, ...) override {
		(void)fmt;
	}


};
}
