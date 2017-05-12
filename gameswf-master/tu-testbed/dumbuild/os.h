// os.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Some routines that interact with the operating system.

#ifndef OS_H_
#define OS_H_

#include "dmb_types.h"
#include "res.h"

// Ensure that the given sub-directory path exists below the given
// absolute root.
Res CreatePath(const string& root, const string& sub_path);

// Try to erase the specified file.
Res EraseFile(const string& path);

bool FileExists(const string& path);

bool DirExists(const string& dirpath);

bool ExeExists(const string& dirpath);

// Execute a sub-process.  dir gives the current directory of the
// subprocess; cmd_line gives the command line with arguments.
//
// environment is an optional set of environment variable strings to
// give to the new process.  Pass an empty string to inherit this
// process' environment.  The environment is encoded as a set of
// null-terminated strings inside the string, the whole thing
// terminated with an extra '\0'.  Each string is of the form
// "VARNAME=VALUE" (sans quotes).
//
// On success, Res.Ok() is true.  On failure, the return value
// contains error details.
Res RunCommand(const string& dir,
               const string& cmd_line,
               const string& environment);

string GetCurrentDir();
Res ChangeDir(const char* newdir);

// Collect the subdirectories of the given path.
Res GetSubdirectories(const string& path, vector<string>* out);

#endif  // OS_H_
