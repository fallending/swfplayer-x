



class A
{
	int	m_a;

	static void	some_static_function();
};


/*static*/ void	A::some_static_function()
{
	typedef int MY_TEST_TYPE;
	MY_TEST_TYPE test_var;
}


void	some_other_function()
{
	typedef bool MY_TEST_TYPE;

	MY_TEST_TYPE test_var;
}
