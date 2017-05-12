// file_deps.cpp -- Thatcher Ulrich <tu@tulrich.com> 2009

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include <set>
#include <stdio.h>
#include "file_deps.h"
#include "config.h"
#include "context.h"
#include "hash_util.h"
#include "object_store.h"
#include "os.h"
#include "target.h"
#include "util.h"

// #include path search order (observed in both cygwin gcc 3.4.4 and
// Visual Studio 2005):
//
// * quoted include paths (e.g. #include "myfile.h") are first
//   searched for relative to the path of the file that has the
//   #include directive.  Then they are searched for in the list of
//   paths from -I options.
//
// * bracketed include paths (e.g. #include <myfile.h>) are searched
//   for in the list of paths from -I options.
//
// * paths in -I options are expressed relative to the current working
//   dir of the compiler process.

// Return true and fill *header_file if we found an #include line.
// Set *is_quoted to true if the file was surrounded by double-quotes;
// set to false if it was surrounded by angle brackets.
bool ParseIncludeLine(const char* line, string* header_file, bool* is_quoted) {
  assert(header_file);
  assert(is_quoted);

  // Skip leading spaces.
  while (char c = *line) {
    if (c != ' ' && c != '\t') {
      break;
    }
    line++;
  }

  if (line[0] != '#') {
    // Not a preprocessor directive.
    return false;
  }
  line++;

  // Skip more spaces.
  while (char c = *line) {
    if (c != ' ' && c != '\t') {
      break;
    }
    line++;
  }

  if (strncmp("include", line, 7) != 0) {
    // No #include on this line.
    return false;
  }
  line += 7;

  for (;;) {
    char c = *line;
    if (c == 0) {
      return false;
    }
    if (c != ' ' && c != '\t') {
      if (c == '"' || c == '<') {
        if (c == '"') {
          *is_quoted = true;
        }
        // Looks like the #include filename is here.
        *line++;
        const char* p = line;
        while (char d = *p) {
          if ((*is_quoted && d == '"') || (*is_quoted == false && d == '>')) {
            // End of filename.
            header_file->assign(line, p - line);
            return true;
          }
          p++;
        }
      }
      // Bad format.
      return false;
    }
    line++;
  }
  return false;
}

// Searches for an existing header file.  Returns true and fills in
// *header_path if it finds one.
bool FindHeader(const string& src_dir, const Target* t,
                const Context* context, const string& header_file,
                bool is_quoted, string* header_path) {
  // TODO: handle absolute header file names,
  // (e.g. #include "/abs/path/header.h" or "c:/somefile.h")

  if (is_quoted) {
    // First look in the directory where the source file was found.
    string path = PathJoin(src_dir, header_file);
    if (FileExists(path)) {
      *header_path = path;
      return true;
    }
  }

  for (size_t i = 0; i < t->inc_dirs().size(); i++) {
    // TODO: handle absolute inc_dirs
    string path = PathJoin(context->tree_root(), t->inc_dirs()[i]);
    path = PathJoin(path, header_file);
    if (FileExists(path)) {
      *header_path = path;
      return true;
    }
  }
  return false;
}

Res GetIncludes(const Target* t, const Context* context,
                const string& src_path, const string& inc_dirs_str,
                vector<string>* includes) {
  Hash src_hash;
  Res res = context->ComputeOrGetFileContentHash(src_path, &src_hash);
  if (!res.Ok()) {
    return res;
  }

  Hash deps_file_id;
  deps_file_id << "includes" << src_hash << inc_dirs_str;

  ObjectStore* ostore = context->GetObjectStore();
  FILE* fp = ostore->Read(deps_file_id);
  if (fp) {
    // Read the deps file.
    static const int BUFSIZE = 1000;
    char linebuf[BUFSIZE];
    while (fgets(linebuf, BUFSIZE, fp)) {
      if (linebuf[0] == '\r' || linebuf[0] == '\n' || linebuf[0] == '#') {
        // Skip.
        continue;
      }
      int len = strlen(linebuf);

      // Trim.
      while (len >= 0 && (linebuf[len - 1] == '\n' ||
                          linebuf[len - 1] == '\r')) {
        linebuf[len - 1] = 0;
        len--;
      }

      includes->push_back(linebuf);
    }
    fclose(fp);
  } else {
    // Scan the source file.
    FILE* fp_src = fopen(src_path.c_str(), "rb");
    if (!fp_src) {
      return Res(ERR_FILE_ERROR, "Couldn't open file for include scanning: " +
                 src_path);
    }

    string src_dir = FilenamePathPart(src_path);
    static const int BUFSIZE = 1000;
    char linebuf[BUFSIZE];
    string header_file;
    string header_path;
    bool is_quoted = false;
    while (fgets(linebuf, BUFSIZE, fp_src)) {
      if (ParseIncludeLine(linebuf, &header_file, &is_quoted)) {
        if (FindHeader(src_dir, t, context, header_file, is_quoted,
                       &header_path)) {
          includes->push_back(header_path);
        }
      }
    }
    fclose(fp_src);

    // Write the deps file.
    FILE* fp = ostore->Write(deps_file_id);
    if (!fp) {
      context->LogVerbose("Unable to write deps to ostore for file: " +
                          src_path);
    } else {
      fprintf(fp, "# includes %s\n", src_path.c_str());
      for (size_t i = 0; i < includes->size(); i++) {
        const string& header_path = (*includes)[i];
        bool ok =
          (fwrite(header_path.c_str(), header_path.size(), 1, fp) == 1) &&
          (fputc('\n', fp) != EOF);
        if (!ok) {
          fclose(fp);
          return Res(ERR_FILE_ERROR, "Error writing to deps file for " +
                     src_path);
        }
      }
      fclose(fp);
    }
  }

  return Res(OK);
}

// DepHash(src_file) = Hash(src_file) + sum(for d in deps: DepHash(d))
//
// The include_chain helps us avoid infinite recursion in case of
// include cycles.
Res AccumulateSrcFileDepHash(const Target* t, const Context* context,
                             const string& src_path, const string& inc_dirs_str,
                             std::set<string>* included,
                             Hash* dep_hash) {
  if (included->find(src_path) != included->end()) {
    // Recursive include; we've already visited this file.  Don't
    // recurse infinitely!
    *dep_hash << src_path;
    return Res(OK);
  }

  // Insert src_path into the include chain, and remove it before we
  // return.
  struct AutoInsertRemove {
   public:
    AutoInsertRemove(std::set<string>* included, const string& file) :
        included_(included), file_(file) {
      included_->insert(file_);
    }
    ~AutoInsertRemove() {
      included_->erase(file_);
    }
   private:
    std::set<string>* included_;
    const string& file_;
  };
  AutoInsertRemove auto_insert_remove(included, src_path);

  Hash content_hash;
  Res res = context->ComputeOrGetFileContentHash(src_path, &content_hash);
  if (!res.Ok()) {
    return res;
  }

  // Check to see if we've already computed this dep hash.
  HashCache<Hash>* dep_hash_cache = context->GetDepHashCache();
  assert(dep_hash_cache);
  Hash dep_cache_key;
  dep_cache_key << "dep_cache_key" << src_path << content_hash << inc_dirs_str;
  Hash cached_dep_hash;
  if (dep_hash_cache->Get(dep_cache_key, &cached_dep_hash)) {
    *dep_hash << cached_dep_hash;
    return Res(OK);
  }

  // Compute the dep hash.
  Hash computed_dep_hash;
  computed_dep_hash << content_hash;

  vector<string> includes;
  res = GetIncludes(t, context, src_path, inc_dirs_str, &includes);
  if (!res.Ok()) {
    return res;
  }

  for (size_t i = 0; i < includes.size(); i++) {
    res = AccumulateSrcFileDepHash(t, context, includes[i], inc_dirs_str,
                                   included, &computed_dep_hash);
    if (!res.Ok()) {
      return res;
    }
  }

  // Write the computed hash back to the cache, for re-use.
  dep_hash_cache->Insert(dep_cache_key, computed_dep_hash);
  *dep_hash << computed_dep_hash;

  return Res(OK);
}

// DepHash(obj_file) = Hash(compile_flags & environment) + DepHash(src_file)
Res AccumulateObjFileDepHash(const Target* t, const Context* context,
                             const string& src_path,
                             const string& inc_dirs_str, Hash* dep_hash) {
  const Config* config = context->GetConfig();
  *dep_hash << src_path << inc_dirs_str << config->compile_environment()
            << config->prefilled_compile_template();

  std::set<string> included;
  return AccumulateSrcFileDepHash(t, context, src_path, inc_dirs_str,
                                  &included, dep_hash);
}
