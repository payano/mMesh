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

#include "DebugInterface.h"
#include <stdint.h>

namespace syscalls {
class SyscallsInterface;
}

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
	syscalls::SyscallsInterface *syscalls;
public:
#ifndef UNIX
	STM32Debug(syscalls::SyscallsInterface *syscalls);
#else
	STM32Debug(void *uart){
		(void)uart;
	}
#endif

private:
	int get_argument(const char **arg);
	void add_int(char **buf_ptr, uint32_t value, int base, int signedval);
	void add_string(char **buffer, const char *string);
	void logger(char* buffer_ptr, const char *fmt, void *argument);

public:

	void error(const char *fmt, ...) override;
	void warn(const char *fmt, ...) override;
	void info(const char *fmt, ...) override;
	void debug(const char *fmt, ...) override;
};
}
