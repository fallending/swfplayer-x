// hash.h -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Thin wrapper around stb's sha1 code.

#ifndef HASH_H_
#define HASH_H_

#include <assert.h>
#include <string.h>
#include "dmb_types.h"
#include "res.h"

class Hash {
 public:
  Hash();

  bool InitFromReadable(const char readable[27]);
  
  void operator=(const Hash& b);
  void Reset();
  Res AppendFile(const char* filename);
  Res AppendFile(const string& filename);
  void AppendData(const char* data, int size);
  void AppendString(const string& str);
  void AppendInt(int i);
  void AppendStringVec(const vector<string>& string_vec);
  void AppendHash(const Hash& h) {
    AppendData((const char*) h.data(), h.size());
  }

  // Shorthand, so you can do things like:
  //   Hash h; h << "key_id" << some_string << some_hash;
  Hash& operator<<(const string& str) {
    AppendString(str);
    return *this;
  }
  Hash& operator<<(const char* str) {
    AppendData(str, strlen(str));
    return *this;
  }
  Hash& operator<<(const Hash& h) {
    AppendHash(h);
    return *this;
  }
  Hash& operator<<(int i) {
    AppendInt(i);
    return *this;
  }
  Hash& operator<<(const vector<string>& string_vec) {
    AppendStringVec(string_vec);
    return *this;
  }

  bool operator==(const Hash& b) const;
  bool operator!=(const Hash& b) const {
    return !(*this == b);
  }
  bool operator<(const Hash& b) const;

  const unsigned char* data() const {
    return &h_[0];
  }
  unsigned char* mutable_data() {
    return &h_[0];
  }
  int size() const {
    return sizeof(h_);
  }

  void GetReadable(char readable[27]);

 private:
  unsigned char h_[20];  // sha1
};

// Helper class. Holds a map from a key type to a computed hash.
template<class T>
class HashCache {
 public:
  void Insert(const T& key, const Hash& h) {
    map_.insert(std::make_pair(key, h));
  }

  bool Get(const T& key, Hash* h) {
    assert(h);
    typename map<T, Hash>::const_iterator it = map_.find(key);
    if (it != map_.end()) {
      *h = it->second;
      return true;
    }
    return false;
  }

 private:
  map<T, Hash> map_;
};

#endif  // HASH_H_
