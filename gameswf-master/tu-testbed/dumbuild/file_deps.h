// file_deps.h -- Thatcher Ulrich <tu@tulrich.com> 2009

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Utilities for checking C/C++ inter-file dependencies (i.e. tracking
// #include files).

#ifndef FILE_DEPS_H_
#define FILE_DEPS_H_

#include "dmb_types.h"
#include "res.h"

class Hash;
class Context;
class Target;

Res AccumulateObjFileDepHash(const Target* t, const Context* context,
                             const string& src_path,
                             const string& inc_dirs_str, Hash* dep_hash);

// DepHash == Cumulative Dependency Hash ==
//   cumulative hash of all content+config that goes into some build product
//
// DepHash(src_file) = Hash(src_file) + Hash(inc_dirs) +
//     sum(for i in includes: DepHash(i))
//
// When DepHash(file) changes, anything that depends on file must be
// rebuilt.  DepHash() can also be used as a reliable id for a build
// product (i.e. as a key to a built-products cache).

// DepHash(obj_file) = Hash(compile_flags & environment) + DepHash(src_file)

// DepHash(lib_or_exe_file) = Hash(link_flags & environment) +
//    sum(d in deps: DepHash(d)) +

// A build cache would be: map<DepHash, content>

// Deps(obj_file): implicitly, src_file

// Deps(lib_file or exe_file):
//   list of obj files
//   list of libraries
// (these come directly out of build.dmb files)

// Build marker: under obj_or_lib_or_exe_file_name.hash
//   DepHash(file)

// Rebuild rule:
//   rebuild(target) = BuildMarker(target) != DepHash(target)

//Res GetDepHashForObjectFile(const Target* t, const Context* context,
//			      const string& src_path);

//Res AccumulateDepsDepHash(, hash* h);

//Res AppendDeps(src_file, FILE* fp_out);

#endif  // FILE_DEPS_H_
