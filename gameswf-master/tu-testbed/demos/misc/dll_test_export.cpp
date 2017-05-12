// compile with "cl /MD /LD dll_test_export.cpp"

#include <stdio.h>

extern "C" __declspec(dllexport) void test_exported()
{
	printf("Hello from test_exported!\n");
}
