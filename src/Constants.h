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
static constexpr int CHILDREN_SZ(5);
static constexpr int NEIGHBOUR_SZ(10);
static constexpr int NUM_ADDRESSES(7); // 2^3
static constexpr int MSG_BUFFER(10);
static constexpr int MAX_HOPS(12);
static constexpr int NET_COUNT(9);
static constexpr int PAYLOAD_SZ(32);

static constexpr int TIMER_COUNTER_PING_NB(6);
static constexpr int TIMER_COUNTER_PING_PARENT(6);
static constexpr int TIMER_COUNTER_BC_NB(10);
static constexpr int TIMER_COUNTER_BC_PARENT(10);
static constexpr int TIMER_KEEPALIVE(3000);
static constexpr int TIMER_DECREASE(100);
