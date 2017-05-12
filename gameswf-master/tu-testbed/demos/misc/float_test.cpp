// test float precision


#include <stdio.h>


float dot_product(const float a[3], const float b[3])
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}


float dot_product_with_float_intermediates(const float a[3], const float b[3])
{
	volatile float	x_sub;
	volatile float	y_sub;
	volatile float	z_sub;

	x_sub = a[0] * b[0];
	y_sub = a[1] * b[1];
	z_sub = a[2] * b[2];

	volatile float	sub_1 = x_sub + y_sub;
	volatile float	sub_2 = sub_1 + z_sub;

	return sub_2;
}


int main()
{
	float a[3] = { 1.001f, 1000, 1000 };
	float b[3] = { 1.1f, 1000, -1000 };

	printf("normal dot = %f\n", dot_product(a, b));
	printf("int dot    = %f\n", dot_product_with_float_intermediates(a, b));

	return 0;
}

// compile with, for example:
//
// g++ -O2 float_test.cpp -o float_test
