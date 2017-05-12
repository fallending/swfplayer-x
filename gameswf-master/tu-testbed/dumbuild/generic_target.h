// generic_target.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// A GenericTarget has configuration, dependencies and sources.

#ifndef GENERIC_TARGET_H_
#define GENERIC_TARGET_H_

#include "target.h"

class GenericTarget : public Target {
 public:
  GenericTarget() {
    set_type("generic");
  }

  Res Init(const Context* context,
           const string& name,
           const Json::Value& val) {
    return Target::Init(context, name, val);
  }
  
  virtual Res Resolve(Context* context) {
    context->LogVerbose(StringPrintf("GenericTarget Resolve: %s\n",
                                     name().c_str()));
    return Target::Resolve(context);
  }

  virtual Res Process(const Context* context) {
    if (processed()) {
      return Res(OK);
    }

    context->LogVerbose(StringPrintf("GenericTarget Process: %s\n",
                                     name().c_str()));
    processed_ = true;
    return Res(OK);
  }
};

#endif  // GENERIC_TARGET_H_
