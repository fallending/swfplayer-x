// external_lib_target.h -- Thatcher Ulrich <tu@tulrich.com> 2009

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#ifndef EXTERNAL_LIB_TARGET_H_
#define EXTERNAL_LIB_TARGET_H_

#include "target.h"

class ExternalLibTarget : public Target {
 public:
  ExternalLibTarget();
  Res Init(const Context* context,
	   const string& name,
	   const Json::Value& val);
  virtual Res Resolve(Context* context);
  virtual Res Process(const Context* context);

  virtual string GetLinkerArgs(const Context* context) const;
};

#endif  // EXTERNAL_LIB_TARGET_H_
