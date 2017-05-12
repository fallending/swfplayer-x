// test.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include <stdio.h>
#include "test.h"

void RunSelfTests() {
  TestPath();
  TestEval();
  TestUtil();
  TestOs();
}
