// Template overloading experiments

// xxx test
template<class T, class U> struct myclass
{
	void	gen() { /* fully generic */ }
};

template<class U> struct myclass<float, U> { void gen() { /* partial float, U */ } };

template<class U> struct myclass<int, U> { void gen() { /* partial int, U */ } };



class Type1;
class Type2;
class Type3;


template<class PtrType>
struct generator
{
	static void f(Type1* p);
};


template<class PtrType>
template<class T>
/*static*/ void generator<PtrType>::f() { /* something; */ }


template<class PtrType>
template<>
/*static*/ void generator<PtrType>::f<int>() { /* something; */ }

