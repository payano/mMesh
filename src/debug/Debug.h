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

#include "main.h"
#include "DebugInterface.h"
#include "DataTypes.h"


namespace debugger {

class Debug : public DebugInterface {
private:
	UART_HandleTypeDef *huart;
public:
#ifndef UNIX
	Debug(UART_HandleTypeDef *uart)
	{
		huart = uart;
	}


#if DEBUG_LEVEL	== DEBUG
	template<typename T, typename ... Args>
	void debug(char *fmt, T first, Args ... args) {
		char final_string[100];
		mem_clr(final_string, 100);
		char *prefix = (char *)"DEBUG: ";
		char *suffix = (char *)"\n\r";

		/* Add prefix */
		int prefix_sz = string_len(prefix);
		char *final_ptr = final_string + prefix_sz;
		copy_data(final_string, prefix, prefix_sz);

		log(final_ptr, fmt, first, args ...);

		/* Add suffix */
		int suffix_sz = string_len(suffix);
		char *last_pos = final_string;
		last_pos += string_len(final_string);
		copy_data(last_pos, suffix, suffix_sz);

		HAL_UART_Transmit(huart, (uint8_t*)final_string, string_len(final_string), 100);
	}
#endif
#if DEBUG_LEVEL == DEBUG || DEBUG_LEVEL == WARN
	template<typename T, typename ... Args>
	void warn(char *fmt, T first, Args ... args) {
		char final_string[100];
		mem_clr(final_string, 100);
		char *prefix = (char *)"WARN: ";
		char *suffix = (char *)"\n\r";

		/* Add prefix */
		int prefix_sz = string_len(prefix);
		char *final_ptr = final_string + prefix_sz;
		copy_data(final_string, prefix, prefix_sz);

		log(final_ptr, fmt, first, args ...);

		/* Add suffix */
		int suffix_sz = string_len(suffix);
		char *last_pos = final_string;
		last_pos += string_len(final_string);
		copy_data(last_pos, suffix, suffix_sz);

		HAL_UART_Transmit(huart, (uint8_t*)final_string, string_len(final_string), 100);
	}
#endif

#if DEBUG_LEVEL == DEBUG || DEBUG_LEVEL == WARN || DEBUG_LEVEL == INFO
	template<typename T, typename ... Args>
	void info(char *fmt, T first, Args ... args) {
		char final_string[100];
		mem_clr(final_string, 100);
		char *prefix = (char *)"INFO: ";
		char *suffix = (char *)"\n\r";

		/* Add prefix */
		int prefix_sz = string_len(prefix);
		char *final_ptr = final_string + prefix_sz;
		copy_data(final_string, prefix, prefix_sz);

		log(final_ptr, fmt, first, args ...);

		/* Add suffix */
		int suffix_sz = string_len(suffix);
		char *last_pos = final_string;
		last_pos += string_len(final_string);
		copy_data(last_pos, suffix, suffix_sz);

		HAL_UART_Transmit(huart, (uint8_t*)final_string, string_len(final_string), 100);
	}
#endif

private:
	template<typename T, typename ... Args>
	static void log(char *final_string, char *fmt, T first, Args ... args) {
		while(*fmt != '%' && *fmt != 0) {
			*final_string = *fmt;
			++final_string;
			++fmt;
		}
		if(0 == *fmt) return;

		char datatype[4];
		mem_clr(datatype, 4);
		char *data_ptr = datatype;

		++fmt;
		//	while(*fmt != ' ' && *fmt != 0) {
		while(*fmt >= 48) {
			*data_ptr = *fmt;
			++fmt;
			++data_ptr;
		}

		if(0 == cmp_data(datatype, "u32", 3)){
			/* signed 32 bit */
			int32_t value;

			memcpy(&value, &first, sizeof(int32_t));
			int ff = int_to_ascii(value, final_string, 10, false);
			final_string += ff;

		} else if(0 == cmp_data(datatype, "s32", 3)) {
			/* unsigned 32 bit */
			uint32_t value;

			memcpy(&value, &first, sizeof(uint32_t));
			int ff = int_to_ascii(value, final_string, 10, true);
			final_string += ff;

		} else if(0 == cmp_data(datatype, "s", 1)) {
			/* string */
			const char *data_str = (const char*)first;
			int sz = string_len(data_str);
			for(int i = 0; i < sz; ++i){
				*final_string = *data_str;
				++final_string;
				++data_str;
			}
		}

		// Forward the remaining arguments
		log(final_string, fmt, args ...);

		/* print last bytes aswell*/
	}

	__attribute__((unused))
	static int int_to_ascii(uint32_t value, char *buffer, int base, bool signedval)
	{
		/* if zero just return zero */
		if(0 == value) {
			*buffer = '0';
			++buffer;
			return 0;
		}

		char rev_val[10];
		mem_clr(rev_val, 10);
		char *data_ptr = rev_val;
		int val;
		int count = 0;
		bool negative = false;

		if(signedval) {
			int32_t sval;
			copy_data((uint32_t*)&sval, &value, sizeof(value));
			if(sval < 0) {
				negative = true;
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

		return count;
	}
	/*
	https://thispointer.com/c11-variadic-template-function-tutorial-examples/
	*/
	// Function that accepts no parameter
	// It is to break the recursion chain of vardiac template function
	__attribute__((unused))
	static void log(char *final_string,  char *fmt)
	{
		while(*final_string != 0) {
			++final_string;
		}

		while(*fmt != 0) {
			*final_string = *fmt;
			++final_string;
			++fmt;
		}
	}
#else
	void log_debug(char *fmt, T first, Args ... args) {
		(void) fmt;
		(void) first;
		(void) args;
	}

#endif

};
}
