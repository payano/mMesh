/*
 * LinuxSyscalls.cpp
 *
 *  Created on: Apr 6, 2020
 *      Author: evsejho
 */
#ifdef UNIX

#include "LinuxSyscalls.h"
#include <chrono>
#include <unistd.h>
#include <stdlib.h>

namespace syscalls {

LinuxSyscalls::LinuxSyscalls() {
	timerStarted = false;
	timerDone = false;
	mThread = nullptr;
}

LinuxSyscalls::~LinuxSyscalls() {
	if(nullptr != mThread) {
		mThread->join();
		delete mThread;
		mThread = nullptr;
	}
}

void LinuxSyscalls::set_cpu_speed(SPEED speed) {
	(void)speed;
}


void LinuxSyscalls::microsleep(int delay)
{
	usleep(delay);
}

void LinuxSyscalls::msleep(int delay)
{
	usleep(1000*delay);
}

void LinuxSyscalls::timerCallback(int ms)
{
	timerDone = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	timerDone = true;
	timerStarted = false;
}

int LinuxSyscalls::start_timer(int delay)
{
	/* Only one at the time */
	if(timerStarted) return 0;
	if(nullptr != mThread) {
		mThread->join();
		delete mThread;
		mThread = nullptr;
	}
	/* this should access a hw timer */
	timerStarted = true;
	mThread = new std::thread(&LinuxSyscalls::timerCallback, this, delay);
	return 1;
}

bool LinuxSyscalls::timer_started()
{
	return timerStarted;
}

int LinuxSyscalls::get_random()
{
	return rand();
}

void LinuxSyscalls::armTimer(int ms)
{
	timerDone = false;
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	timerDone = true;
	timerStarted = false;
}

} /* namespace syscalls */

#endif
