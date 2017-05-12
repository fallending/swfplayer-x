// object_store.h -- Thatcher Ulrich <tu@tulrich.com> 2009

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// A GIT-inspired content-addressable object store.  Addresses/keys
// are sha1 hashes; contents are files.

#ifndef OBJECT_STORE_H_
#define OBJECT_STORE_H_

#include "dmb_types.h"

class Hash;

class ObjectStore {
 public:
  explicit ObjectStore(const char* root_path);

  FILE* Read(const Hash& key) const;
  FILE* Write(const Hash& key);
  bool Exists(const Hash& key) const;
  // Returns true if it erased something.
  bool Erase(const Hash& key);

 private:
  void MakePath(const Hash& key, string* path,
                string* subdir) const;

  string root_path_;
};

#endif  // OBJECT_STORE_H_
