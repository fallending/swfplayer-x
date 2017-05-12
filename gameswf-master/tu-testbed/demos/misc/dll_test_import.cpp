// compile with "cl /MD dll_test_import.cpp dll_test_export.lib"

extern "C" __declspec(dllexport) test_exported();


int main()
{
	test_exported();

	return 0;
}
