// target.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Base class for dumbuild Target type.  A Target has a canonical
// name, a type, and dependencies.  Subclasses define different
// behavior.
//
// There are basically three stages of Target processing:
//
// 1. Init() -- construct the Target based on its .dmb declaration data.
//
// 2. Resolve() -- make sure dependencies are known/loaded and resolved.
//
// 3. Process() -- make sure dependencies are processed, then do the
//    build action for this target.

#ifndef TARGET_H_
#define TARGET_H_

#include <assert.h>
#include <json/json.h>

#include "context.h"
#include "dmb_types.h"
#include "hash.h"
#include "object.h"
#include "path.h"
#include "res.h"

class Target : public Object {
 public:
  Target();
  
  virtual Res Init(const Context* context,
		   const string& name,
		   const Json::Value& value);

  virtual Target* CastToTarget() {
    return this;
  }

  bool resolved() const {
    return resolved_;
  }

  bool processed() const {
    return processed_;
  }

  // List of src files.
  const vector<string>& src() const {
    return src_;
  }
	
  // List of other targets we depend on.
  const vector<string>& dep() const {
    return dep_;
  }

  // List of include dirs for compiling this target.
  const vector<string>& inc_dirs() const {
    return inc_dirs_;
  }

  // List of include dirs for compiling targets that depend on this
  // target.
  const vector<string>& dep_inc_dirs() const {
    return dep_inc_dirs_;
  }

  // Per-target cflags.
  const string& target_cflags() const {
    return target_cflags_;
  }

  // cflags for targets that depend on us.
  const string& dep_cflags() const {
    return dep_cflags_;
  }

  // Relative path from the target's compile output directory back up
  // to the tree root.
  const string& relative_path_to_tree_root() const {
    return relative_path_to_tree_root_;
  }

  // Absolute path to the target's compile output directory.
  const string& absolute_out_dir() const {
    return absolute_out_dir_;
  }

  // List of linker flags necessary when linking with this target.
  const string& linker_flags() const {
    return linker_flags_;
  }

  // List of additional libs to add to linker command line of linkable
  // targets that depend on this target.  (Mainly applicable to
  // external_lib targets.)
  const vector<string>& dep_libs() const {
    return dep_libs_;
  }

  virtual Res Resolve(Context* context);

  // Helper: does standard processing of dependencies.
  // Returns Res(OK) on success.
  Res ProcessDependencies(const Context* context);

  // Helper: does standard setup of paths & makes the output
  // directory.
  Res BuildOutDirAndSetupPaths(const Context* context);

  // Build the target.
  virtual Res Process(const Context* context) = 0;

  // The dep_hash is a hash of everything that goes into a target.  If
  // the target changes at all, then the dep hash will change.
  const Hash& dep_hash() const {
    assert(dep_hash_was_set_);
    return dep_hash_;
  }

  virtual string GetLinkerArgs(const Context* context) const;

 private:
  Res BaseDirSearcher(const Context* context, const Json::Value& val);

 protected:
  bool resolved_, processed_, dep_hash_was_set_;
  int resolve_recursion_;
  vector<string> src_, dep_, inc_dirs_, dep_inc_dirs_, dep_libs_;
  string target_cflags_;
  string dep_cflags_;
  string relative_path_to_tree_root_;
  string absolute_out_dir_;
  string linker_flags_;
  Hash dep_hash_;
};

#endif  // TARGET_H_
