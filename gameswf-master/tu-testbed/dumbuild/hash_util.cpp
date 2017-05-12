// hash_util.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include <stdio.h>
#include "hash_util.h"
#include "path.h"
#include "util.h"

// Given a file path, normalize it to a valid local filename that
// represents the path.
static string LocalFileStemFromPath(const string& path) {
  // TODO: for now we're just taking the file part, but in the future
  // we might want to take the whole path and normalize it to a valid
  // local file name (i.e. replace slashes and ".." with other
  // characters).
  return FilenameFilePart(path);
}

Res WriteFileHash(const string& out_dir,
		   const string& file_path,
		   const Hash& hash) {
  string hash_fname = PathJoin(out_dir, LocalFileStemFromPath(file_path));
  hash_fname += ".hash";

  FILE* f = fopen(hash_fname.c_str(), "wb");
  if (!f) {
    return Res(ERR_FILE_ERROR, "WriteFileHash can't open file " + hash_fname);
  } else {
    size_t wrote = fwrite(hash.data(), hash.size(), 1, f);
    if (wrote != 1) {
      return Res(ERR_FILE_ERROR, "WriteFileHash couldn't write to file " +
                 hash_fname);
    }
    int closed_code = fclose(f);
    if (closed_code != 0) {
      return Res(ERR_FILE_ERROR, "WriteFileHash couldn't close " + hash_fname);
    }
  }

  return Res(OK);
}

Res ReadFileHash(const string& out_dir,
                 const string& file_path,
                 Hash* hash) {
  string hash_fname = PathJoin(out_dir, LocalFileStemFromPath(file_path));
  hash_fname += ".hash";

  hash->Reset();
  FILE* f = fopen(hash_fname.c_str(), "rb");
  if (f) {
    if (fread((void*) hash->data(), hash->size(), 1, f)) {
      fclose(f);
      return Res(OK);
    } else {
      fclose(f);
      hash->Reset();
      return Res(ERR_FILE_ERROR, "ReadFileHash: " + hash_fname);
    }
  }
  return Res(ERR_FILE_ERROR, "ReadFileHash: file open failed: " + hash_fname);
}
