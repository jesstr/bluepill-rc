#include <stdio.h>
#include "tinystdio.h"
#include <string.h>


void my_printf(const char* fmt,  ...)
{

	char buffer[100];
	va_list args;
	va_start (args, fmt);

	tfp_vsprintf(buffer, fmt, args);

	printf("%s", buffer);
	va_end (args);

}

void print_test(int test_nr, const char* fmt, ...)
{
	char stdio_buff[100];
	char tiny_buff[100];

	va_list args;
	va_start (args, fmt);

	tfp_vsprintf(tiny_buff, fmt, args);
	vsprintf(stdio_buff, fmt, args);

	if (strcmp(stdio_buff, tiny_buff) != 0)
	{
		printf("%d. Test failed.\n\rExpected: %s\n\rGot:      %s\n\r\n\r",test_nr,  stdio_buff, tiny_buff);
	}
	else
		printf("%d. Test passed.\n\r\t%s\n\r\n\r", test_nr, tiny_buff);

	va_end (args);

}

int test_printf(void)
{
    /*
    1. Test passed.
            Hello, my friend.

    2. Test passed.
            This is decimal number 00053

    3. Test passed.
            512 2312 000666    553

    4. Test passed.
            Floats     2.1 5.33 2.312000, 6.6600000 0003.21

    5. Test passed.
            Written in HEX! 0004E 23 ffffffec      5 0000098

    6. Test passed.
            Pointer address! 0x07b

    7. Test failed.
    Expected: 0.41200  -0.02 0.000231 -1.123400 -0.666687 0.300000
    Got:      0.41200  -0.02 0.000231 -1.123399 -0.666687 0.30
    */

	printf("Running test:\n\r");

	print_test(1, "Hello, my friend.");
	print_test(2, "This is decimal number %05d", 53);
	print_test(3, "%d %03d %06d %6d", 512, 2312, 666, 553, 3223);
	print_test(4, "Floats %7.1f %3.2f %05.6f, %05.7f %07.2f", 2.1, 5.33, 2.3120001, 6.66, 3.215);
	print_test(5, "Written in HEX! %05X %x %06x %6x %07X", 78, 35, -20, 5, 152);
	print_test(6, "%s address! %05p", "Pointer", 123);
	print_test(7, "%3.5f %6.2f %2.6f %03.6f %2.6f %f", 0.412, -0.02, 0.000231, -1.1234, -0.666687, 0.3);


	return 0;
}
