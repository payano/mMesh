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

#include "DebugSingleton.h"
#include "DebugInterface.h"
#include "LinuxDebug.h"
#include "STM32Debug.h"

namespace debugger {
DebugInterface *DebugSingleton::debugger = nullptr;

void DebugSingleton::createLinuxInstance()
{
	if(DebugSingleton::debugger != nullptr) return;
	DebugSingleton::debugger = new LinuxDebug();
}

void DebugSingleton::createSTM32Instance(UART_HandleTypeDef *huart1)
{
	if(DebugSingleton::debugger != nullptr) return;
	DebugSingleton::debugger = new STM32Debug(huart1);
}


//
//DebugInterface *getInstance()
//{
//	return DebugSingleton::debugger;
//}

}
