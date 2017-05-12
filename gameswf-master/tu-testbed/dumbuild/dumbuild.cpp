// dumbuild.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// C++ build tool written in C++.  See README.txt for more info.

#include <assert.h>
#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#else  // not _WIN32
#include <unistd.h>
#endif  // not _WIN32

#include "config.h"
#include "context.h"
#include "dmb_types.h"
#include "eval.h"
#include "os.h"
#include "res.h"
#include "target.h"
#include "util.h"

void PrintUsage() {
  printf(
      "dmb [options] [target]\n"
      "\n"
      "The target is found by looking for a build.dmb file in the specified\n"
      "target's path, and then looking for the named target within that\n"
      "build.dmb.  For example:\n"
      "\n"
      "  dmb libs/base/math\n"
      "\n"
      "will try to parse the file \"libs/base/build.dmb\" for a target\n"
      "named \"math\", and build it according to the target's\n"
      "specification.  If not specified, the default target is \"default\".\n"
      "\n"
      "options include:\n"
      "\n"
      "  -h           Print usage.\n"
      "\n"
      "  -C <dir>     Change to the specified directory before starting work.\n"
      "               This should have the effect of invoking dmb from that\n"
      "               directory.\n"
      "\n"
      "  -c <config>  Specify the name of a build configuration (i.e. compiler \n"
      "               & mode).  Supplied configurations in the default root.dmb\n"
      "               include\n"
      "                  gcc-debug\n"
      "                  gcc-release\n"
      "                  vc8-debug\n"
      "                  vc8-release\n"
      "                  vc9-debug\n"
      "                  vc9-release\n"
      "               If not specified, the default configuration is \n"
      "               \"default\", which tries to do some auto-detection for\n"
      "               an appropriate build configuration.\n"
      "\n"
      "  -r           Rebuild all, whether or not source files have changed.\n"
      "\n"
      "  -v           Verbose.  Does a lot of extra logging.\n"
      "\n"
      "  --test       Run internal unit tests.\n"
      "\n"
      "The project root directory is located by searching upward from the\n"
      "current directory for a file named \"root.dmb\".  root.dmb may\n"
      "contain project-wide defaults.  Target paths may be specified in\n"
      "relation to the project root.\n"
      "\n"
      "The build output goes in <project-root>/dmb-out/<config-name>/\n"
      "\n"
      "For more info see http://tulrich.com/geekstuff/dumbuild/\n"
         );
}

void ExitIfError(const Res& res, const Context& context) {
  if (res.Ok()) {
    return;
  }

  if (res.value() == ERR_SHOW_USAGE) {
    PrintUsage();
    exit(1);
  }

  fprintf(stderr, "dmb error\n");
  fprintf(stderr, "%s\n", res.ToString().c_str());

  if (context.log_verbose()) {
    context.LogAllTargets();
  }

  exit(1);
}

// On success, currdir_relative_to_root will be a canonical path.
Res FindRoot(string* absolute_root,
             string* currdir_relative_to_root) {
  string currdir = GetCurrentDir();

  // Look for root dir.  Root dir is marked by the presence of a
  // "root.dmb" file.
  string root = currdir;
  for (;;) {
    if (FileExists(PathJoin(root, "root.dmb"))) {
      break;
    }
    if (HasParentDir(root)) {
      root = ParentDir(root);
    } else {
      return Res(ERR, "Can't find root.dmb in ancestor directories");
    }
  }
  *absolute_root = root;

  const char* remainder = currdir.c_str() + root.length();
  if (remainder[0] == '/') {
    remainder++;
  }
  *currdir_relative_to_root = remainder;
  assert(currdir_relative_to_root->size() == 0 ||
         (*currdir_relative_to_root)[currdir_relative_to_root->length() - 1]
         != '/');

  return Res(OK);
}

void InitStatics() {
  InitEval();
}

int main(int argc, const char** argv) {
  InitStatics();

  Context context;
  Res res;

  res = context.ProcessArgs(argc, argv);
  ExitIfError(res, context);

  string absolute_root;
  string canonical_currdir;
  res = FindRoot(&absolute_root, &canonical_currdir);
  ExitIfError(res, context);

  context.LogVerbose(StringPrintf("absolute_root = %s\n",
				  absolute_root.c_str()));
  context.LogVerbose(StringPrintf("canonical_currdir = %s\n",
				  canonical_currdir.c_str()));

  res = context.Init(absolute_root, canonical_currdir);
  ExitIfError(res, context);

  assert(context.GetConfig());
  context.Log(StringPrintf("dmb config: %s\n",
			   context.GetConfig()->name().c_str()));

  res = context.Resolve();
  ExitIfError(res, context);

  res = context.ProcessTargets();
  ExitIfError(res, context);

  context.Log("dmb OK\n");

  return 0;
}
