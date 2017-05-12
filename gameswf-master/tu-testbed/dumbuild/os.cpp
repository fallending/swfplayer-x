// os.cpp -- Thatcher Ulrich <tu@tulrich.com> 2008

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "os.h"
#include "util.h"

#ifdef _WIN32

#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <direct.h>

Res CreatePath(const string& root, const string& sub_path) {
  string current = root;
  const char* prev = sub_path.c_str();
  for (;;) {
    const char* next_slash = strchr(prev, '/');
    if (next_slash) {
      current += "/";
      current += string(prev, next_slash - prev);
    } else {
      current += "/";
      current += prev;
    }
    if (!CreateDirectory(current.c_str(), NULL)) {
      DWORD last_error = GetLastError();
      if (last_error != ERROR_ALREADY_EXISTS) {
        // TODO add last_error to res detail.
        return Res(ERR, "CreatePath " + sub_path + " failed.");
      }
    }

    if (!next_slash) {
      break;
    }
    prev = next_slash + 1;
  }

  return Res(OK);
}

Res RunCommand(const string& dir, const string& cmd_line,
               const string& environment) {
  PROCESS_INFORMATION proc_info;
  memset(&proc_info, 0, sizeof(proc_info));

  STARTUPINFO startup_info;
  memset(&startup_info, 0, sizeof(startup_info));
  startup_info.cb = sizeof(startup_info);
  startup_info.dwFlags = STARTF_USESTDHANDLES;
  startup_info.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
  startup_info.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  startup_info.hStdError = GetStdHandle(STD_ERROR_HANDLE);

  BOOL retval = CreateProcess(
          NULL,
          (LPSTR) cmd_line.c_str(),
          NULL, NULL,
          TRUE,  /* inherit handles */
          0,
          environment.length() ? (LPVOID) environment.c_str() : NULL,
          dir.c_str(),
          &startup_info,
          &proc_info);
  if (!retval) {
    return Res(ERR_SUBCOMMAND_FAILED, "Failed to invoke " + cmd_line);
  }

  DWORD wait_res = WaitForSingleObject(proc_info.hProcess, INFINITE);
  if (wait_res != WAIT_OBJECT_0) {
    return Res(ERR, "RunCommand: WaitForSingleObject failed.");
  }

  DWORD exit_code = 0;
  if (!GetExitCodeProcess(proc_info.hProcess, &exit_code)) {
    return Res(ERR, "RunCommand: GetExitCodeProcess failed.");
  }
  if (exit_code != 0) {
    // TODO: add the exit code to the error detail.
    return Res(ERR_SUBCOMMAND_FAILED,
               StringPrintf("RunCommand returned non-zero exit status 0x%X:"
                            "\n>>%s", exit_code, cmd_line.c_str()));
  }

  return Res(OK);
}

#else // not _WIN32

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

Res CreatePath(const string& root, const string& sub_path) {
  string current = root;
  const char* prev = sub_path.c_str();
  for (;;) {
    const char* next_slash = strchr(prev, '/');
    if (next_slash) {
      current += "/";
      current += string(prev, next_slash - prev);
    } else {
      current += "/";
      current += prev;
    }
    if (mkdir(current.c_str(), 0755) != 0) {
      if (errno != EEXIST) {
        // TODO add last_error to res detail.
        return Res(ERR, StringPrintf("CreatePath '%s' failed, errno = %d", errno));
      }
    }

    if (!next_slash) {
      break;
    }
    prev = next_slash + 1;
  }

  return Res(OK);
}

Res RunCommand(const string& dir, const string& cmd_line,
               const string& environment) {
  // Split command line on spaces, ignoring any quoting.
  //
  // TODO(tulrich): might be good to support quoting someday, to allow
  // spaces inside individual args.
  vector<string> args;
  {
    const char* p = cmd_line.c_str();
    for (;;) {
      while (*p == ' ') p++;  // skip leading spaces.
      const char* n = strchr(p, ' ');
      assert(n != p);
      if (!n) {
        // last arg.
        if (*p) {
          args.push_back(p);
        }
        break;
      } else {
        args.push_back(string(p, n - p));
      }
      p = n + 1;
    }
  }

  // Set up program, argv and envp.
  const char* program = NULL;
  vector<const char*> argv;
  vector<const char*> envp;
  for (size_t i = 0; i < args.size(); i++) {
    argv.push_back(args[i].c_str());
  }
  argv.push_back(NULL);

  if (argv.size()) {
    program = argv[0];
  }

  {
    const char* p = environment.c_str();
    const char* e = environment.c_str() + environment.size();
    while (*p && p < e) {
      envp.push_back(p);
      p += strlen(p) + 1;
    }
    envp.push_back(NULL);
  }
  char* const *argv_p = (char* const*) &argv[0];
  char* const *envp_p = (char* const*) &envp[0];

  // Fork.
  pid_t pid = fork();
  if (pid == 0) {
    // Child process.
    if (chdir(dir.c_str()) == 0) {
      execve(program, argv_p, envp_p);
    }
    _exit(1);
  }

  // Parent process.
  if (pid == -1) {
    return Res(ERR_SUBCOMMAND_FAILED,
               StringPrintf("RunCommand failed to fork, errno = %d\n>>",
                            errno, cmd_line.c_str()));
  }
  int status = 0;
  if (waitpid(pid, &status, 0) != pid) {
    return Res(ERR_SUBCOMMAND_FAILED,
               StringPrintf("RunCommand failed to get status, "
                            "errno = %d, cmd =\n>>%s",
                            errno, cmd_line.c_str()));
  }
  if (status != 0) {
    return Res(ERR_SUBCOMMAND_FAILED,
               StringPrintf("RunCommand returned non-zero exit status "
                            "0x%X:\n>>%s", status, cmd_line.c_str()));
  }

  return Res(OK);
}

#endif  // not _WIN32


#ifdef _WIN32
#define GETCWD _getcwd
#define CHDIR _chdir
#define UNLINK _unlink
#else
#define GETCWD getcwd
#define CHDIR chdir
#define UNLINK unlink
#endif

string GetCurrentDir() {
  // Allocate a big return buffer for getcwd.
  const int MAX_CURRDIR_SIZE = 2000;
  string currdir;
  currdir.resize(MAX_CURRDIR_SIZE);
  if (!GETCWD(&currdir[0], currdir.size())) {
    fprintf(stderr, "Internal error: getcwd() larger than %d\n",
            MAX_CURRDIR_SIZE);
    exit(1);
  }
  // Trim to the correct size.
  size_t sz = strlen(currdir.c_str());
  currdir.resize(sz);

#ifdef _WIN32
  // Change backslashes to forward slashes.
  for (size_t pos = currdir.find('\\');
       pos != string::npos;
       pos = currdir.find('\\', pos)) {
    currdir[pos] = '/';
  }
#endif  // _WIN32

  return currdir;
}

Res ChangeDir(const char* newdir) {
  if (CHDIR(newdir) == 0) {
    return Res(OK);
  }

  return Res(ERR, StringPrintf("chdir(\"%s\") failed", newdir));
}

bool FileExists(const string& path) {
  struct stat stat_info;
  int err = stat(path.c_str(), &stat_info);
  if (err) {
    return false;
  }
  return true;
}

bool DirExists(const string& path) {
  struct stat stat_info;
  int err = stat(path.c_str(), &stat_info);
  if (err) {
    return false;
  }
  return (stat_info.st_mode & S_IFDIR) != 0;
}

bool ExeExists(const string& path) {
  struct stat stat_info;
  int err = stat(path.c_str(), &stat_info);
  if (err) {
    return false;
  }
#ifdef _WIN32
  string extension = GetExt(path);
  return stricmp(extension.c_str(), "exe") == 0 ||
	  stricmp(extension.c_str(), "dll") == 0;
#else  // not _WIN32
  return (stat_info.st_mode & S_IXUSR) != 0;
#endif  // not _WIN32
}

Res EraseFile(const string& path) {
  if (UNLINK(path.c_str()) == 0) {
    return Res(OK);
  }
  return Res(ERR_FILE_ERROR, "Can't unlink " + path);
}

Res GetSubdirectories(const string& path, vector<string>* out) {
  assert(out);
  out->resize(0);
  string dot_dir = ".";
  string dotdot_dir = "..";

#ifdef _WIN32
  WIN32_FIND_DATA find_data;
  string path_arg = path;
  if (path_arg.size() > 0 && (path_arg[path_arg.size() - 1] == '/' ||
                              path_arg[path_arg.size() - 1] == '\\')) {
    path_arg.resize(path_arg.size() - 1);
  }
  path_arg += "\\*";
  HANDLE h = FindFirstFile(path_arg.c_str(), &find_data);
  if (h == INVALID_HANDLE_VALUE) {
    return Res(ERR_FILE_ERROR, "GetSubdirectories(): couldn't open directory " +
               path);
  }
  do {
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
        dot_dir != find_data.cFileName &&
        dotdot_dir != find_data.cFileName) {
      out->push_back(find_data.cFileName);
    }
  } while (FindNextFile(h, &find_data) != 0);

  DWORD err = GetLastError();
  if (err != ERROR_NO_MORE_FILES) {
    return Res(ERR_FILE_ERROR, StringPrintf("GetSubdirectories(): windows "
                                            "error 0x%X", err));
  }
  FindClose(h);

#else  // not _WIN32
  DIR* dp;
  struct dirent* ep;
  dp = opendir((path + "/").c_str());
  if (dp == NULL) {
    return Res(ERR_FILE_ERROR, "GetSubdirectories(): couldn't open directory " +
               path);
  } else {
    while ((ep = readdir(dp))) {
      if (ep->d_type & DT_DIR &&
          dot_dir != ep->d_name && dotdot_dir != ep->d_name) {
        out->push_back(ep->d_name);
      }
    }
    closedir(dp);
  }
#endif  // not _WIN32

  std::sort(out->begin(), out->end());

  return Res(OK);
}

void TestGetSubdirectories() {
#ifdef _WIN32
  vector<string> dirs;
  Res res = GetSubdirectories("c:/", &dirs);
  assert(res.Ok());
  assert(std::find(dirs.begin(), dirs.end(), "Windows") != dirs.end());
  assert(std::find(dirs.begin(), dirs.end(), "Documents and Settings") !=
         dirs.end());

  res = GetSubdirectories("c:/Windows", &dirs);
  assert(res.Ok());
  assert(std::find(dirs.begin(), dirs.end(), "Fonts") != dirs.end());
  assert(std::find(dirs.begin(), dirs.end(), "system") !=
         dirs.end());

#else  // not _WIN32
  vector<string> dirs;
  Res res = GetSubdirectories("/", &dirs);
  assert(std::find(dirs.begin(), dirs.end(), "usr") != dirs.end());
  assert(std::find(dirs.begin(), dirs.end(), "etc") != dirs.end());
#endif  // not _WIN32
}

void TestOs() {
  TestGetSubdirectories();
}
