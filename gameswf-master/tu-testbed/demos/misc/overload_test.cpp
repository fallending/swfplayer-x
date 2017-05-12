#include <stdio.h>


struct Stream
{
	FILE*	m_out;

	Stream(FILE* out) : m_out(out) {}
};


struct SomeStructType
{
	int	m_int;
	float	m_float;

	SomeStructType() : m_int(27), m_float(1.2f) {}
};


// Builtins.
void	operator^(int i, Stream& str) { fprintf(str.m_out, "int: %d\n", i); }
void	operator^(float f, Stream& str) { fprintf(str.m_out, "float: %g\n", f); }

// User-defined type.  Could be implemented by a member function.
void	operator^(SomeStructType& x, Stream& str)
{
	x.m_int ^ str;
	x.m_float ^ str;
}

class SomeBase
{
public:
		virtual void operator^(Stream& str) = 0; // here's my virtual
};


class SomeDerived : public SomeBase
{
public:
	float	m_myMember;
	SomeStructType	m_myStruct;

	SomeDerived() : m_myMember(42.42f) {}

	virtual void operator^(Stream& str)
	{
		m_myMember ^ str;
		m_myStruct ^ str;
	}
};


int main()
{
	Stream	out(stdout);

	int		i(10);
	SomeStructType	s;
	SomeDerived	sd;

	// Call sites all look the same, even though they're implemented as
	// a mixture of non-members, plain members, and virtual members.
	i ^ out;
	s ^ out;
	sd ^ out;

	return 0;
}
