// hash.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include "dmb_types.h"
#include "hash.h"
#include "sha1.h"
#include "util.h"

Hash::Hash() {
  Reset();
}

void Hash::Reset() {
  stb_sha1_init(h_);
}

bool Hash::InitFromReadable(const char readable[27]) {
  return stb_sha1_from_readable(readable, h_);
}

Res Hash::AppendFile(const char* filename) {
  if (!stb_sha1_file(h_, filename)) {
    Reset();
    return Res(ERR_FILE_ERROR, StringPrintf("Can't get file hash of '%s'",
                                            filename));
  }
  return Res(OK);
}

Res Hash::AppendFile(const string& filename) {
  return AppendFile(filename.c_str());
}

void Hash::AppendData(const char* data, int size) {
  stb_sha1(h_, (const unsigned char*) data, size);
}

void Hash::AppendString(const string& str) {
  AppendData(str.c_str(), str.length());
}

void Hash::AppendInt(int i) {
  AppendData((const char*) &i, sizeof(i));
}

void Hash::AppendStringVec(const vector<string>& string_vec) {
  AppendInt(string_vec.size());
  for (size_t i = 0; i < string_vec.size(); i++) {
    AppendString(string_vec[i]);
  }
}

void Hash::operator=(const Hash& b) {
  memcpy(h_, b.h_, sizeof(h_));
}

bool Hash::operator==(const Hash& b) const {
  for (size_t i = 0; i < sizeof(h_); i++) {
    if (h_[i] != b.h_[i]) {
      return false;
    }
  }
  return true;
}

bool Hash::operator<(const Hash& b) const {
  for (size_t i = 0; i < sizeof(h_); i++) {
    if (h_[i] < b.h_[i]) {
      return true;
    } else if (h_[i] > b.h_[i]) {
      return false;
    } // else bytes are equal, keep checking.
  }
  // They're equal.
  return false;
}

void Hash::GetReadable(char readable[27]) {
  stb_sha1_readable(readable, h_);
}
