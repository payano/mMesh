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

#include "SyscallsInterface.h"

namespace syscalls {

void SyscallsInterface::copy_data(void *to, const void *from, int sz){
	uint8_t *to_d = static_cast<uint8_t*>(to);
	const uint8_t *from_d = static_cast<const uint8_t*>(from);
	int arch_sz = sizeof(void*);
	int ret;

	switch(arch_sz) {
	case ARCH64:
		ret = copy_data8(to_d, from_d, &sz);
		if(sz == 0) return;
		to_d += ret;
		from_d += ret;
		/* FALLTHROUGH */
	case ARCH32:
		ret = copy_data4(to_d, from_d, &sz);
		if(sz == 0) return;
		to_d += ret;
		from_d += ret;
		/* FALLTHROUGH */
	case ARCH16:
		ret = copy_data2(to_d, from_d, &sz);
		if(sz == 0) return;
		to_d += ret;
		from_d += ret;
		/* FALLTHROUGH */
	case ARCH8:
		/* FALLTHROUGH */
	default:
		ret = copy_data1(to_d, from_d, &sz);
		if(sz == 0) return;
	}
}

int SyscallsInterface::cmp_data(const void *first, const void *second, int sz) {
	const uint8_t *first_d = static_cast<const uint8_t*>(first);
	const uint8_t *second_d = static_cast<const uint8_t*>(second);
	int arch_sz = sizeof(void*);
	int compared;
	int ret;

	switch(arch_sz) {
	case ARCH64:
		ret = cmp_data8(first_d, second_d, &sz, &compared);
		if(ret > 0) return -1; /* Not a perfect match */
		if(sz == 0) return 0; /* Perfect match */
		first_d += compared;
		second_d += compared;
		/* FALLTHROUGH */
	case ARCH32:
		ret = cmp_data4(first_d, second_d, &sz, &compared);
		if(ret > 0) return -1; /* Not a perfect match */
		if(sz == 0) return 0; /* Perfect match */
		first_d += compared;
		second_d += compared;
		/* FALLTHROUGH */
	case ARCH16:
		ret = cmp_data2(first_d, second_d, &sz, &compared);
		if(ret > 0) return -1; /* Not a perfect match */
		if(sz == 0) return 0; /* Perfect match */
		first_d += compared;
		second_d += compared;
		/* FALLTHROUGH */
	case ARCH8:
		/* FALLTHROUGH */
	default:
		ret = cmp_data1(first_d, second_d, &sz, &compared);
		if(ret > 0) return -1; /* Not a perfect match */
		if(sz == 0) return 0; /* Perfect match */
	}
	return -1;	}

void SyscallsInterface::mem_clr(void *buffer, int sz) {
	uint8_t *buffer_d = static_cast<uint8_t*>(buffer);
	int arch_sz = sizeof(void*);
	int ret;

	switch(arch_sz) {
	case ARCH64:
		ret = mem_clr8(buffer_d, &sz);
		if(sz == 0) return;
		buffer_d += ret;
		/* FALLTHROUGH */
	case ARCH32:
		ret = mem_clr4(buffer_d, &sz);
		if(sz == 0) return;
		buffer_d += ret;
		/* FALLTHROUGH */
	case ARCH16:
		ret = mem_clr2(buffer_d, &sz);
		if(sz == 0) return;
		buffer_d += ret;
		/* FALLTHROUGH */
	case ARCH8:
		/* FALLTHROUGH */
	default:
		ret = mem_clr1(buffer_d, &sz);
		if(sz == 0) return;
	}
}

int SyscallsInterface::string_len(const void *string_in)
{
	int counter = 0;
	const uint8_t *str = (const uint8_t *)string_in;
	while(*str != '\0') {
		++counter;
		++str;
	}
	return counter;
}

int SyscallsInterface::copy_data8(void *to, const void *from, int *sz)
{
	uint64_t *to_d = static_cast<uint64_t*>(to);
	const uint64_t *from_d = static_cast<const uint64_t*>(from);
	int ret = 0;
	while(*sz > 0 && *sz - ARCH64 >= 0) {
		*to_d = *from_d;
		++to_d;
		++from_d;
		*sz -= ARCH64;
		ret += ARCH64;
	}
	return ret;
}

int SyscallsInterface::copy_data4(void *to, const void *from, int *sz)
{
	uint32_t *to_d = static_cast<uint32_t*>(to);
	const uint32_t *from_d = static_cast<const uint32_t*>(from);
	int ret = 0;

	while(*sz > 0 && *sz - ARCH32 >= 0) {
		*to_d = *from_d;
		++to_d;
		++from_d;
		*sz -= ARCH32;
		ret += ARCH32;
	}
	return ret;
}

int SyscallsInterface::copy_data2(void *to, const void *from, int *sz)
{
	uint16_t *to_d = static_cast<uint16_t*>(to);
	const uint16_t *from_d = static_cast<const uint16_t*>(from);
	int ret = 0;

	while(*sz > 0 && *sz - ARCH16 >= 0) {
		*to_d = *from_d;
		++to_d;
		++from_d;
		*sz -= ARCH16;
		ret += ARCH16;
	}
	return ret;
}

int SyscallsInterface::copy_data1(void *to, const void *from, int *sz)
{
	uint8_t *to_d = static_cast<uint8_t*>(to);
	const uint8_t *from_d = static_cast<const uint8_t*>(from);
	int ret = 0;

	while(*sz > 0 && *sz - ARCH8 >= 0) {
		*to_d = *from_d;
		++to_d;
		++from_d;
		*sz -= ARCH8;
		ret += ARCH8;
	}
	return ret;
}

int SyscallsInterface::cmp_data8(const void *first, const void *second, int *sz, int *compared)
{
	const uint64_t *first_d = static_cast<const uint64_t*>(first);
	const uint64_t *second_d = static_cast<const uint64_t*>(second);
	*compared = 0;
	while(*sz > 0 && *sz - ARCH64 >= 0) {
		if(*first_d != *second_d) return -1;
		++first_d;
		++second_d;
		*sz -= ARCH64;
		*compared += ARCH64;
	}
	return 0;
}

int SyscallsInterface::cmp_data4(const void *first, const void *second, int *sz, int *compared)
{
	const uint32_t *first_d = static_cast<const uint32_t*>(first);
	const uint32_t *second_d = static_cast<const uint32_t*>(second);
	*compared = 0;
	while(*sz > 0 && *sz - ARCH32 >= 0) {
		if(*first_d != *second_d) return -1;
		++first_d;
		++second_d;
		*sz -= ARCH32;
		*compared += ARCH32;
	}
	return 0;
}

int SyscallsInterface::cmp_data2(const void *first, const void *second, int *sz, int *compared)
{
	const uint16_t *first_d = static_cast<const uint16_t*>(first);
	const uint16_t *second_d = static_cast<const uint16_t*>(second);
	*compared = 0;
	while(*sz > 0 && *sz - ARCH16 >= 0) {
		if(*first_d != *second_d) return -1;
		++first_d;
		++second_d;
		*sz -= ARCH16;
		*compared += ARCH16;
	}
	return 0;
}

int SyscallsInterface::cmp_data1(const void *first, const void *second, int *sz, int *compared)
{
	const uint8_t *first_d = static_cast<const uint8_t*>(first);
	const uint8_t *second_d = static_cast<const uint8_t*>(second);
	*compared = 0;
	while(*sz > 0 && *sz - ARCH8 >= 0) {
		if(*first_d != *second_d) return -1;
		++first_d;
		++second_d;
		*sz -= ARCH8;
		*compared += ARCH8;
	}
	return 0;
}

int SyscallsInterface::mem_clr8(void *buffer, int *sz)
{
	uint64_t *buffer_d = static_cast<uint64_t*>(buffer);
	int ret = 0;
	while(*sz > 0 && *sz - ARCH64 >= 0) {
		*buffer_d = 0;
		++buffer_d;
		*sz -= ARCH64;
		ret += ARCH64;
	}
	return ret;
}

int SyscallsInterface::mem_clr4(void *buffer, int *sz)
{
	uint32_t *buffer_d = static_cast<uint32_t*>(buffer);
	int ret = 0;
	while(*sz > 0 && *sz - ARCH32 >= 0) {
		*buffer_d = 0;
		++buffer_d;
		*sz -= ARCH32;
		ret += ARCH32;
	}
	return ret;
}

int SyscallsInterface::mem_clr2(void *buffer, int *sz)
{
	uint16_t *buffer_d = static_cast<uint16_t*>(buffer);
	int ret = 0;
	while(*sz > 0 && *sz - ARCH16 >= 0) {
		*buffer_d = 0;
		++buffer_d;
		*sz -= ARCH16;
		ret += ARCH16;
	}
	return ret;
}

int SyscallsInterface::mem_clr1(void *buffer, int *sz)
{
	uint8_t *buffer_d = static_cast<uint8_t*>(buffer);
	int ret = 0;
	while(*sz > 0 && *sz - ARCH8 >= 0) {
		*buffer_d = 0;
		++buffer_d;
		*sz -= ARCH8;
		ret += ARCH8;
	}
	return ret;
}

}
