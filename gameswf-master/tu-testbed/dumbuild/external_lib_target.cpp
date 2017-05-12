// external_lib_target.cpp -- Thatcher Ulrich <tu@tulrich.com> 2009

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include "external_lib_target.h"
#include "compile_util.h"
#include "config.h"
#include "hash_util.h"
#include "os.h"
#include "util.h"

ExternalLibTarget::ExternalLibTarget() {
  set_type("external_lib");
}

Res ExternalLibTarget::Init(const Context* context, const string& name,
                    const Json::Value& val) {
  Res res = Target::Init(context, name, val);
  return res;
}

Res ExternalLibTarget::Resolve(Context* context) {
  context->LogVerbose(StringPrintf("ExternalLibTarget Resolve: %s\n",
				   name().c_str()));
  // Default Resolve() is OK.
  return Target::Resolve(context);
}

Res ExternalLibTarget::Process(const Context* context) {
  dep_hash_ << "external_lib_dep_hash" << name();
  dep_hash_ << dep();
  dep_hash_ << linker_flags();
  dep_hash_ << inc_dirs();
  dep_hash_was_set_ = true;

  processed_ = true;
  return Res(OK);
}

string ExternalLibTarget::GetLinkerArgs(const Context* context) const {
  return string();
}
