// lib_target.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#ifndef LIB_TARGET_H_
#define LIB_TARGET_H_

#include "target.h"

class LibTarget : public Target {
 public:
  LibTarget();
  Res Init(const Context* context,
	   const string& name,
	   const Json::Value& val);
  virtual Res Resolve(Context* context);
  virtual Res Process(const Context* context);

  virtual string GetLinkerArgs(const Context* context) const;
};

#endif  // LIB_TARGET_H_
