// target.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include "target.h"
#include "config.h"
#include "eval.h"
#include "os.h"
#include "util.h"

Target::Target()
    : resolved_(false), processed_(false), dep_hash_was_set_(false),
      resolve_recursion_(0) {
}

static Res ParseCanonicalPathList(const string& base_dir, const Json::Value& list,
                                   vector<string>* dirs) {
  if (!list.isObject() && !list.isArray()) {
    return Res(ERR_PARSE, "value is not object or array: " +
               list.toStyledString());
  }

  for (Json::Value::iterator it = list.begin();
       it != list.end();
       ++it) {
    if (!(*it).isString()) {
      return Res(ERR_PARSE, "list value is not a string: " +
                 (*it).toStyledString());
    }
    string dir_name = Canonicalize(base_dir, (*it).asString());
    dirs->push_back(dir_name);
  }
  return Res(OK);
}

Res Target::Init(const Context* context,
                 const string& target_name,
                 const Json::Value& value) {
  Res res = Object::Init(context, target_name, value);
  if (!res.Ok()) {
    return res;
  }

  context->LogVerbose(StringPrintf("Target::Init(): %s\n", name().c_str()));

  if (value.isMember("base_dir")) {
    Json::Value val = value["base_dir"];
    set_base_dir(Canonicalize(name_dir(), val.asString()));
  }

  if (value.isMember("base_dir_inherit")) {
    string inherit_from_name = value["base_dir_inherit"].asString();
    Target* inherit_from = context->GetTarget(inherit_from_name);
    if (!inherit_from) {
      return Res(ERR_PARSE, name() + ": base_dir_inherit can't find target " +
                 inherit_from_name);
    }
    set_base_dir(inherit_from->base_dir());
  }

  if (value.isMember("base_dir_searcher")) {
    Json::Value val = value["base_dir_searcher"];
    res = BaseDirSearcher(context, val);
    if (!res.Ok()) {
      return res;
    }
  }

  // Initialize dependencies.
  if (value.isMember("dep")) {
    Json::Value deplist = value["dep"];
    if (!deplist.isObject() && !deplist.isArray()) {
      return Res(ERR_PARSE,
                 name() + ": dep value is not object or array: " +
                 deplist.toStyledString());
    }

    for (Json::Value::iterator it = deplist.begin();
         it != deplist.end();
         ++it) {
      if (!(*it).isString()) {
        return Res(ERR_PARSE, name() + ": dep list value is not a string: " +
                   (*it).toStyledString());
      }
      string depname = Canonicalize(name_dir(), (*it).asString());

      dep_.push_back(depname);
    }
  }

  // Initialize source list.
  if (value.isMember("src")) {
    Json::Value srclist = value["src"];
    if (!srclist.isObject() && !srclist.isArray()) {
      return Res(ERR_PARSE,
                 name() + ": src value is not object or array: " +
                 srclist.toStyledString());
    }

    for (Json::Value::iterator it = srclist.begin();
         it != srclist.end();
         ++it) {
      if (!(*it).isString()) {
        return Res(ERR_PARSE, name() + ": src list value is not a string: " +
                   (*it).toStyledString());
      }
      string srcname = Canonicalize(base_dir(), (*it).asString());
      src_.push_back(srcname);
    }
  }

  // Initialize include dirs.
  if (value.isMember("inc_dirs")) {
    Json::Value inclist = value["inc_dirs"];
    res = ParseCanonicalPathList(base_dir(), inclist, &inc_dirs_);
    if (!res.Ok()) {
      res.AppendDetail("while parsing inc_dirs in " + name());
      return res;
    }
  }
  // TODO: collect additional inc dirs from dependencies (?).

  // Initialize include dirs that should be passed on to dependents.
  if (value.isMember("dep_inc_dirs")) {
    Json::Value dep_inclist = value["dep_inc_dirs"];
    res = ParseCanonicalPathList(base_dir(), dep_inclist, &dep_inc_dirs_);
    if (!res.Ok()) {
      res.AppendDetail("while parsing dep_inc_dirs in " + name());
      return res;
    }
  }

  // Initialize libs that should be linked with dependents.
  if (value.isMember("dep_libs")) {
    Json::Value dep_liblist = value["dep_libs"];
    res = ParseCanonicalPathList(base_dir(), dep_liblist, &dep_libs_);
    if (!res.Ok()) {
      res.AppendDetail("while parsing dep_libs in " + name());
      return res;
    }
  }

  if (value.isMember("linker_flags")) {
    Json::Value srclist = value["linker_flags"];
    res = ParseValueStringOrMap(context, NULL, srclist, "=", ";",
                                &linker_flags_);
    if (!res.Ok()) {
      res.AppendDetail("\nwhile evaluating linker_flags field of target " +
                       name());
      return res;
    }
  }

  if (value.isMember("target_cflags")) {
    Json::Value val = value["target_cflags"];
    res = ParseValueStringOrMap(context, NULL, val, "=", ";",
                                &target_cflags_);
    if (!res.Ok()) {
      res.AppendDetail("\nwhile evaluating target_cflags field of target " +
                       name());
      return res;
    }
  }

  if (value.isMember("dep_cflags")) {
    Json::Value val = value["dep_cflags"];
    res = ParseValueStringOrMap(context, NULL, val, "=", ";",
                                &dep_cflags_);
    if (!res.Ok()) {
      res.AppendDetail("\nwhile evaluating dep_cflags field of target " +
                       name());
      return res;
    }
  }

  return Res(OK, "");
}

Res Target::Resolve(Context* context) {
  ScopedIncrement inc(&resolve_recursion_);

  if (resolve_recursion_ > 1) {
    // Cycle!  Stop the cycle and log the names of targets in the
    // dependency chain.
    return Res(ERR_DEPENDENCY_CYCLE, name());
  }

  if (!resolved()) {
    context->LogVerbose(StringPrintf("Target Resolve: %s\n", name().c_str()));

    // Try to Resolve() all our dependencies.
    for (size_t i = 0; i < dep().size(); i++) {
      Target* dependency = NULL;
      Res res = context->GetOrLoadTarget(dep()[i], &dependency);
      if (!res.Ok()) {
        res.AppendDetail("\nwhile resolving " + name());
        return res;
      }

      assert(dependency);
      res = dependency->Resolve(context);
      if (!res.Ok()) {
        if (res.value() == ERR_DEPENDENCY_CYCLE) {
          // Tack on our name to the error message, so the whole
          // cycle is logged for the user.
          res.AppendDetail("\nreferred to by " + name());
          return res;
        }
        return res;
      }
    }
  }

  resolved_ = true;
  return Res(OK);
}

Res Target::ProcessDependencies(const Context* context) {
  context->LogVerbose(StringPrintf("Target ProcessDependencies: %s\n",
                                   name().c_str()));
  // TODO: once dmb does a topological sort of targets, this can just
  // be a series of asserts to make sure it worked right.
  
  // Try to Process() all our dependencies.
  for (size_t i = 0; i < dep().size(); i++) {
    Target* dependency = context->GetTarget(dep()[i]);
    assert(dependency);
    Res res = dependency->Process(context);
    if (!res.Ok()) {
      return res;
    }
    assert(dependency->processed());
  }

  return Res(OK);
}

Res Target::BuildOutDirAndSetupPaths(const Context* context) {
  string out_dir = PathJoin(context->out_root(), name_dir());
  Res res = CreatePath(context->tree_root(), out_dir);
  if (!res.Ok()) {
    res.AppendDetail("\nwhile creating output path for " + name());
    return res;
  }

  absolute_out_dir_ = PathJoin(context->tree_root(), out_dir);

  relative_path_to_tree_root_ = "../";
  const char* slash = strchr(out_dir.c_str(), '/');
  while (slash && *(slash + 1)) {
    relative_path_to_tree_root_ += "../";
    slash = strchr(slash + 1, '/');
  }

  return res;
}

string Target::GetLinkerArgs(const Context* context) const {
  return "";
}

// Search for a base directory.
Res Target::BaseDirSearcher(const Context* context, const Json::Value& value) {
  // "base_search_paths"
  vector<string> base_search_paths;
  if (value.isMember("base_search_paths")) {
    // TODO: make a helper for extracting Json obj/array into a vector<string>
    Json::Value list = value["base_search_paths"];
    if (!list.isObject() && !list.isArray()) {
      return Res(ERR_PARSE,
                 name() + ": base_search_paths value is not object or array: " +
                 list.toStyledString());
    }

    for (Json::Value::iterator it = list.begin();
         it != list.end();
         ++it) {
      if (!(*it).isString()) {
        return Res(ERR_PARSE, name() + ": base_search_paths value is not "
                              "a string: " +
                              (*it).toStyledString());
      }
      string pathname = Canonicalize(name_dir(), (*it).asString());
      base_search_paths.push_back(context->AbsoluteFile(pathname, ""));
    }
  } else {
    return Res(ERR_PARSE, "base_dir_searcher requires a 'base_search_paths' "
               "member.");
  }

  // "base_patterns"
  vector<string> base_patterns;
  if (value.isMember("base_patterns")) {
    Json::Value list = value["base_patterns"];
    if (!list.isObject() && !list.isArray()) {
      return Res(ERR_PARSE,
                 name() + ": base_patterns value is not object or array: " +
                 list.toStyledString());
    }

    for (Json::Value::iterator it = list.begin();
         it != list.end();
         ++it) {
      if (!(*it).isString()) {
        return Res(ERR_PARSE, name() +
                   ": base_patterns value is not a string: " +
                   (*it).toStyledString());
      }
      base_patterns.push_back((*it).asString());
    }
  } else {
    return Res(ERR_PARSE, "base_dir_searcher requires a 'base_patterns' "
               "member.");
  }

  // "detected"
  Json::Value detected_predicate;
  if (value.isMember("detected")) {
    detected_predicate = value["detected"];
  }
  if (!detected_predicate.isArray()) {
    return Res(ERR_PARSE, name() +
               ": base_dir_searcher -- 'detected' must be an evaluatable array");
  }

  // Look for a valid base dir
  for (size_t i = 0; i < base_search_paths.size(); i++) {
    const string& path = base_search_paths[i];
    // Enumerate dirs under path.
    vector<string> dirs;
    Res res = GetSubdirectories(path, &dirs);
    if (!res.Ok()) {
      res.AppendDetail("\nin BaseDirSearcher");
      context->LogVerbose(res.ToString() + "\n");
      continue;
    }
    for (size_t d = 0; d < dirs.size(); d++) {
      string dir = dirs[d];
      string abs_dir = PathJoin(path, dir);
      context->LogVerbose("BaseDirSearcher candidate " + abs_dir + "\n");
      for (size_t j = 0; j < base_patterns.size(); j++) {
        if (GlobMatch(base_patterns[j], dir)) {
          context->LogVerbose("BaseDirSearcher glob match");

          // Test this dir against the predicate.
          context->LogVerbose("BaseDirSearcher testing candidate " + abs_dir +
                              "\n");
          string out;
          Config config;
          config.SetVar("base_dir", abs_dir);
          Res res = EvalToString(context, &config, detected_predicate, &out);
          if (!res.Ok()) {
            context->LogVerbose("BaseDirSearcher eval error: " + res.ToString()
                                + "\n");
          } else if (out != "[]") {
            // Found it!
            context->LogVerbose("BaseDirSearcher success: " + abs_dir +
                                "\n");
            set_base_dir(abs_dir);
            return Res(OK);
          } else {
            context->LogVerbose("BaseDirSearcher not detected, returned: " +
                                out + "\n");
          }
        }
      }
    }
  }

  return Res(ERR_DIR_NOT_FOUND, "base_dir_searcher for '" + name() +
             "' couldn't find a matching base dir.");
}
