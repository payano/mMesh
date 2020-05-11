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
#ifdef UNIX

#pragma once

#include "SyscallsInterface.h"
#include <thread>
#include <stdint.h>

//#if defined(_GLIBCXX_HAS_GTHREADS) && defined(_GLIBCXX_USE_C99_STDINT_TR1)


namespace syscalls {

class LinuxSyscalls : public SyscallsInterface {
private:
	bool timerStarted;
	bool timerDone;
	std::thread *mThread;
public:
	LinuxSyscalls();
	virtual ~LinuxSyscalls();

	void init() override {}
	void set_cpu_speed(SPEED speed) override;
	void microsleep(int delay) override;
	void msleep(int delay) override;
	int start_timer(int delay) override;
	bool timer_started() override;
	int get_random() override;
private:
	void armTimer(int ms);
	void timerCallback(int ms);
};
} /* namespace syscalls */
//#endif
#endif
