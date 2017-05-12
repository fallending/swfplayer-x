// compile_util.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#ifndef COMPILE_UTIL_H_
#define COMPILE_UTIL_H_

#include "dmb_types.h"
#include "hash.h"
#include "res.h"

class Context;
class Target;

struct CompileInfo {
  CompileInfo() {
  }

  // Variables for template replacement.
  map<string, string> vars_;
  // Specific source files to be compiled.  Expressed relative to
  // output dir.
  vector<string> src_list_;
  // TODO: header_dir list?
};

// Set up the standard variables for building a compiler command line.
// Append the dep hashes of all sources and dependencies to the given
// *dep_hash.
Res PrepareCompileVars(const Target* t, const Context* context,
                       CompileInfo* compile_info, Hash* dep_hash);

Res DoCompile(const Target* t, const Context* context,
              const CompileInfo& compile_info);

#endif  // COMPILE_UTIL_H_
