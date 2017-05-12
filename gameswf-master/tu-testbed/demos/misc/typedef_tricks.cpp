// typedef_tricks.cpp	-- by Thatcher Ulrich http://tulrich.com 2005

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// A trick for associating arbitrary non-negative integers with
// arbitrarily named scoped symbols inside a class namespace, at
// compile-time.  Lets you implement a little compile-time key-value
// system which may be handy for various kinds of C++ compiler
// metaprogramming.
//
// This may be a standard trick, I don't know (it's hard to believe
// that boost hasn't found all possible C++ compiler tricks).

// The basic idea is that the keys are typenames made with typedef,
// and the value is the sizeof() the typedef'd type.  You have to
// declare & define the key/value inside the class declaration, but
// then you can access the value any time later.

// Why would you want to do this?  E.g. I believe you can use it to
// help choose different template specializations based on the class
// type, without the template having to know about the class.
//
// TODO example, and why else?

// Here's how to declare & define the key/value.  Value must be a
// constant int (for sizing the array).  This defines a type
// MyClass::key whose sizeof() should be the value + 1 (we add one
// so that we can store 0 without relying on support for zero-sized
// arrays)
#define DEFINE_KEY(key, value) typedef char KEYVALUE_ ## key ## __[value + 1];

// To retreive, include the correct class scope.
#define GET_KEY(full_key) (sizeof(full_key) - 1)

// Example:
//
// class MyClass {
// public:
// 	// ... some stuff ...
//	DEFINE_KEY(my_special_number__, 1)
// };
//
// ... printf("%d", GET_KEY(MyClass::my_special_number__));
// 
//
// template<class T>
// void do_something(T* t)
// {
//	specialized_do_something< GET_KEY(T::my_special_number__) >(t);
// }
