// exe_target.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include "exe_target.h"
#include "compile_util.h"
#include "config.h"
#include "dmb_types.h"
#include "hash_util.h"
#include "os.h"
#include "util.h"

ExeTarget::ExeTarget() {
  set_type("exe");
}

Res ExeTarget::Init(const Context* context, const string& name,
                    const Json::Value& val) {
  Res res = Target::Init(context, name, val);
  if (res.Ok()) {
    // TODO
    // more exe-specific init???
  }

  return res;
}

Res ExeTarget::Resolve(Context* context) {
	context->LogVerbose(StringPrintf("ExeTarget Resolve: %s\n", name().c_str()));
  // Default Resolve() is OK.
  return Target::Resolve(context);
}

Res BuildLumpFile(const string& lump_file,
                  const vector<string>& src) {
  // TODO
  assert(0);
  return Res(OK);
}

// TODO combine this with ExeTarget::Process which is very similar.
Res ExeTarget::Process(const Context* context) {
  if (processed()) {
    return Res(OK);
  }

  context->Log(StringPrintf("dmb processing exe %s\n", name().c_str()));

  Res res;
  res = Target::ProcessDependencies(context);
  if (!res.Ok()) {
    res.AppendDetail("\nwhile processing dependencies of " + name());
    return res;
  }

  res = BuildOutDirAndSetupPaths(context);
  if (!res.Ok()) {
    return res;
  }

  const Config* config = context->GetConfig();

  // Read existing build marker, if any.
  string output_fname = FilenameFilePart(name()) + config->exe_extension();
  Hash previous_dep_hash;
  res = ReadFileHash(absolute_out_dir(), output_fname, &previous_dep_hash);
  // Ignore return value; we don't care!

  assert(dep_hash_was_set_ == false);
  dep_hash_.Reset();
  dep_hash_ << "exe_dep_hash" << name() << config->prefilled_link_template();

  CompileInfo ci;
  res = PrepareCompileVars(this, context, &ci, &dep_hash_);
  if (!res.Ok()) {
    return res;
  }

  dep_hash_was_set_ = true;

  bool do_build = context->rebuild_all() || (previous_dep_hash != dep_hash_);
  if (do_build) {
    if (ci.src_list_.size()) {
      res = DoCompile(this, context, ci);
      if (!res.Ok()) {
        return res;
      }
    }
  } else {
    if (ci.src_list_.size()) {
      // This is sort of unexpected; let's print something.
      context->Log("warning: exe_target " + name() + " has apparently "
                   "not changed, but some component obj files may "
                   "be missing!\n");
    }
  }

  if (do_build) {
    // Link.
    context->Log(StringPrintf("Linking %s\n", name().c_str()));
    string cmd;
    res = FillTemplate(config->prefilled_link_template(), ci.vars_, false,
                       &cmd);
    if (!res.Ok()) {
      res.AppendDetail("\nwhile preparing linker command line for " + name());
      return res;
    }
    res = RunCommand(absolute_out_dir(), cmd, config->compile_environment());
    if (!res.Ok()) {
      res.AppendDetail("\nwhile linking " + name());
      res.AppendDetail("\nin directory " + absolute_out_dir());
      return res;
    }

    // Write a build marker.
    res = WriteFileHash(absolute_out_dir(), output_fname, dep_hash_);
    if (!res.Ok()) {
      return res;
    }
  }

  processed_ = true;
  return Res(OK);
}
