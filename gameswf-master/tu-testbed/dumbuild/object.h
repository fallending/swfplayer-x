// object.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Base class for dumbuild Config and Target types.  An Object has a
// name, a type, and other type-specific attributes.
//

#ifndef OBJECT_H_
#define OBJECT_H_

#include <assert.h>
#include <json/json.h>

#include "context.h"
#include "dmb_types.h"
#include "path.h"
#include "res.h"

class Config;
class Target;

class Object {
 public:
  // Factory.
  static Res Create(const Context* context,
		    const string& path,
		    const Json::Value& value,
		    Object** object);

  Object() {
  }

  virtual ~Object() {
  }
  
  virtual Res Init(const Context* context,
		   const string& name,
		   const Json::Value& value);

  const string& name() const {
    return name_;
  }

  // The path part of the name (i.e. our path relative to the project
  // root).  Used as a base dir for dep references.
  const string& name_dir() const {
    return name_dir_;
  }

  // Dir used as a base path for src, inc_dirs, dep_inc_dirs, and
  // dep_libs references.  Defaults to name_dir(), but can be
  // overridden.
  const string& base_dir() const {
    return base_dir_;
  }

  const string& type() const {
    return type_;
  }

  virtual Target* CastToTarget() {
    return NULL;
  }

  virtual Config* CastToConfig() {
    return NULL;
  }

 protected:
  void set_base_dir(const char* d) {
    base_dir_ = d;
  }
  void set_base_dir(const string& d) {
    base_dir_ = d;
  }

  void set_type(const char* t) {
    type_ = t;
  }

 private:
  string name_, type_, name_dir_, base_dir_;
};

#endif  // TARGET_H_
