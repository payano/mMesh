//#include <iostream>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>


static int int_to_ascii(uint32_t value, char *buffer, int base, bool signedval)
{
	/* if zero just return zero */
	if(0 == value) {
		*buffer = '0';
		++buffer;
		return 0;
	}

	char rev_val[10];
	memset(rev_val, 0, 10);
	char *data_ptr = rev_val;
	int val;
	int count = 0;
	bool negative = false;

	if(signedval) {
		int32_t sval;
		memcpy(&sval, &value, sizeof(int32_t));
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

	/* take last aswell */
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
void log(char *final_string,  char *fmt)
{
	while(*final_string != 0) {
		++final_string;
	}

//	printf("%s", fmt);
	while(*fmt != 0) {
		*final_string = *fmt;
		++final_string;
		++fmt;
	}
}

/*
 * Variadic Template Function that accepts variable number
 * of arguments of any type.
 */
template<typename T, typename ... Args>
void log(char *final_string, char *fmt, T first, Args ... args) {
	while(*fmt != '%' && *fmt != 0) {
		*final_string = *fmt;
		++final_string;
		++fmt;
	}
	if(0 == *fmt) return;

	char datatype[4];
	memset(datatype, 0, 4);
	char *data_ptr = datatype;

	++fmt;
//	while(*fmt != ' ' && *fmt != 0) {
	while(*fmt >= 48) {
		*data_ptr = *fmt;
		++fmt;
		++data_ptr;
	}

	if(0 == memcmp(datatype, "u32", 3)){
		/* signed 32 bit */
		int32_t value;

		memcpy(&value, &first, sizeof(int32_t));
		int ff = int_to_ascii(value, final_string, 10, false);
		final_string += ff;

	} else if(0 == memcmp(datatype, "s32", 3)) {
		/* unsigned 32 bit */
		uint32_t value;

		memcpy(&value, &first, sizeof(uint32_t));
		int ff = int_to_ascii(value, final_string, 10, true);
		final_string += ff;

	} else if(0 == memcmp(datatype, "s", 1)) {
		/* string */
		const char *data_str = (const char*)first;
		int sz = strlen(data_str);
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

template<typename T, typename ... Args>
static void debug_log(char *fmt, T first, Args ... args) {
	char final_string[100];
	memset(final_string, 0, 100);
	char *prefix = (char *)"DEBUG: ";
	int prefix_sz = strlen(prefix);
	char *final_ptr = final_string + prefix_sz;
	memcpy(final_string, prefix, prefix_sz);
	log(final_ptr, fmt, first, args ...);
	printf("%s\n", final_string);
}

int main() {

	uint32_t val = 512;
	int32_t negval = -15;
	debug_log((char*)"hej hopp %s,mmm %u32,sopa, %s32", "Lets do this!", val, negval);
	return 0;
}
