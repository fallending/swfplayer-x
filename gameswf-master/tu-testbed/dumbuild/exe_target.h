// exe_target.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#ifndef EXE_TARGET_H_
#define EXE_TARGET_H_

#include "target.h"

class ExeTarget : public Target {
 public:
  ExeTarget();
  Res Init(const Context* context,
	   const std::string& name,
	   const Json::Value& val);
  virtual Res Resolve(Context* context);
  virtual Res Process(const Context* context);
};

#endif  // EXE_TARGET_H_
