// test.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// The self tests.

#ifndef TEST_H_
#define TEST_H_

// Exits with non-zero status on failure.
void RunSelfTests();

// Sub-tests; declared here, but implemented in their respective
// source files.
void TestEval();
void TestOs();
void TestPath();
void TestUtil();

#endif  // TEST_H_
