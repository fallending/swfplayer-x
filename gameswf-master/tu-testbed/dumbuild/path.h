// path.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Paths in config files:
//
//   "#" == project root
//   "#subdir" == first level subdir
//   "#subdir1/subdir2" == two levels deep
//   "../something" == relative path
//   "something" == relative path
//
// Canonical path, used internally for target names:
//   a normal path, relative to the project root
//   doesn't end in '/'
//
// Target names:
//   "path/targetname"
//   "targetname"           == targetname in the current dir
//   "../targetname"        == targetname in parent dir
//   "#path/to/targetname"  == targetname relative to root
//   etc

#ifndef PATH_H_
#define PATH_H_

#include "dmb_types.h"
#include "res.h"

string GetPath(const string& filename);
string PathJoin(const string& a, const string& b);

// relative_path might start with '#', and/or have relative elements.
//
// returns a canonical path.
string Canonicalize(const string& base_dir, const string& relative_path);

bool HasParentDir(const string& path);
// Doesn't work correctly on paths that contain ".." elements.
string ParentDir(const string& path);

bool IsFileNamePart(const string& name);
void SplitFileName(const string& name, string* path_part,
                   string* name_part);
string FilenamePathPart(const string& name);
string FilenameFilePart(const string& name);

bool IsAbsolute(const string& name);

#endif  // PATH_H_
