// count_ones.cpp	-- by Thatcher Ulrich http://tulrich.com

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Google Labs aptitude test prob


#include "base/container.h"


// Remember previous solutions.
array<int>	f_memo;


int	count_ones(int i)
// Count the 1's in the decimal representation of i.
{
	int	count = 0;

	while (i >= 10)
	{
		int	digit = i / 10;
		if (digit == 1) count++;

		i -= digit * 10;
	}

	if (i == 1) count++;

	return count;
}


int	func(int number)
// Return the number of ones required to write out all numbers between
// 0 and number.
{
	if (number >= f_memo.size())
	{
		int	old_size = f_memo.size();
		f_memo.resize(number + 1);

		if (old_size == 0)
		{
			f_memo[0] = 0;
			old_size = 1;
		}

		for (int i = old_size; i <= number; i++)
		{
			f_memo[i] = f_memo[i - 1] + count_ones(i);
		}
	}

	return f_memo[number];
}


int	main(int argc, char *argv[])
{
	for (int i = 0; i < 100000000; i++)
	{
		if (func(i) == i)
		{
			printf("[%d] --> %d\n", i, func(i));
		}
	}
// 	for (int i = 0; i < 100; i++)
// 	{
// 		printf("[%d] --> %d\n", i, func(i));
// 	}

	return 0;
}


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
