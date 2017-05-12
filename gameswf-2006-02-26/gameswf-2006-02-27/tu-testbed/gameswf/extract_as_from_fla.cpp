// extract_as_from_fla.cpp	-- Thatcher Ulrich <tu@tulrich.com> 2005

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Try to extract ActionScript source out of a .fla file.
//
// Flash seems to store text as UCS-16, so basically we can try
// removing 0 characters.


#include <stdio.h>


int main()
{
	int char_count = 0;
	for (; !feof(stdin); )
	{
		int c = fgetc(stdin);
		char_count++;
		if (c == 0)
		{
			// ignore
//			printf("zero");
		}
		else
		{
//			fputc(c, stdout);
		}
//		fflush(stdout);
	}

	printf("%d chars\n", char_count);
}
