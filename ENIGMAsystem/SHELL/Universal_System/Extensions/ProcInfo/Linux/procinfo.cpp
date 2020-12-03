/*

 MIT License
 
 Copyright © 2020 Samuel Venable
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
*/

#include <cstdlib>
#include <cstring>

#include "../procinfo.h"

#include <proc/readproc.h>

using std::string;
using std::vector;
using std::to_string;

namespace procinfo {

string path_from_pid(process_t pid) {
  string path;
  string symlink = string("/proc/") + to_string(pid) + string("/exe");
  char *buffer = realpath(symlink.c_str(), NULL);
  path = buffer ? : "";
  free(buffer);
  return path;
}

string cmd_from_pid(process_t pid) {
  string cmd;
  proc_t proc_info;
  if (!pid_exists(pid)) { return ""; }
  memset(&proc_info, 0, sizeof(proc_info));
  PROCTAB *proc = openproc(PROC_FILLCOM | PROC_PID, &pid);
  if (readproc(proc, &proc_info) != 0) {
    char **args = proc_info.cmdline;
    size_t cnta = 0;
    for (; args[cnta] != NULL && strlen(args[cnta]) != 0; cnta++);
    for (size_t i = 0; i < cnta; i++) {
      if (string_has_whitespace(args[i])) {
        cmd += "\"" + string_replace_all(args[i], "\"", "\\\"") + "\"";
      } else {
        cmd += args[i];
      }
      if (args[i + 1] != NULL) {
        cmd += " ";
      }
    }
  }
  closeproc(proc);
  return cmd;
}

string env_from_pid(process_t pid) {
  string env;
  proc_t proc_info;
  if (!pid_exists(pid)) { return ""; }
  memset(&proc_info, 0, sizeof(proc_info));
  PROCTAB *proc = openproc(PROC_FILLENV | PROC_PID, &pid);
  if (readproc(proc, &proc_info) != 0) {
    char **envs = proc_info.environ;
    size_t cnta = 0;
    for (; envs[cnta] != NULL && strlen(envs[cnta]) != 0; cnta++);
    for (size_t i = 0; i < cnta; i++) {
      size_t j = 0;
      vector<string> envVec = string_split_by_first_equalssign(envs[i]);
      for (const string &environ : envVec) {
        if (j == 0) { env += environ; }
        else { env += "=\"" + string_replace_all(environ, "\"", "\\\"") + "\""; }
        j++;
      }
      if (envs[i + 1] != NULL) {
        env += "\n";
      }
    }
  }
  closeproc(proc);
  return env;
}

string cwd_from_pid(process_t pid) {
  string cwd;
  string symlink = string("/proc/") + to_string(pid) + string("/cwd");
  char *buffer = realpath(symlink.c_str(), NULL);
  cwd = buffer ? : "";
  free(buffer);
  return (cwd.back() == '/' || cwd.empty()) ? cwd : cwd + "/";
}

string pids_enum(bool trim_dir, bool trim_empty) {
  proc_t proc_info;
  memset(&proc_info, 0, sizeof(proc_info));
  PROCTAB *proc = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);
  string pids = "PID\tPPID\t";
  pids += trim_dir ? "NAME\n" : "PATH\n";
  while (readproc(proc, &proc_info) != 0) {
    string exe = trim_dir ? 
      name_from_pid(proc_info.tgid) :
      path_from_pid(proc_info.tgid);
    if (!trim_empty || !exe.empty()) {
      pids += to_string(proc_info.tgid) + "\t";
      pids += to_string(proc_info.ppid) + "\t";
      pids += exe + "\n";
    }
  }
  if (pids.back() == '\n')
    pids.pop_back();
  pids += "\0";
  closeproc(proc);
  return pids;
}

enum PIDRES_SPECTYPE {
  PIDRES_SPECNONE,
  PIDRES_SPECFILE,
  PIDRES_SPECPATH,
  PIDRES_SPECBOTH
};

string pids_from_spec(string name, unsigned spec) {
  string pids; proc_t proc_info;
  memset(&proc_info, 0, sizeof(proc_info));
  PROCTAB *proc = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);
  while (readproc(proc, &proc_info) != 0) {
    string exe;
    if (spec == PIDRES_SPECFILE) 
      exe = name_from_pid(proc_info.tgid);
    if (spec == PIDRES_SPECPATH) 
      exe = dir_from_pid(proc_info.tgid);
    if (spec == PIDRES_SPECBOTH) 
      exe = path_from_pid(proc_info.tgid);
    if (name == exe || spec == PIDRES_SPECNONE)
      pids += to_string(proc_info.tgid) + "|";
  }
  if (pids.back() == '|')
    pids.pop_back();
  pids += "\0";
  closeproc(proc);
  return pids;
}

process_t ppid_from_pid(process_t pid) {
  process_t ppid;
  proc_t proc_info;
  memset(&proc_info, 0, sizeof(proc_info));
  PROCTAB *proc = openproc(PROC_FILLSTATUS | PROC_PID, &pid);
  if (readproc(proc, &proc_info) != 0) { 
    ppid = proc_info.ppid;
  }
  closeproc(proc);
  return ppid;
}

string pids_from_ppid(process_t ppid) {
  string pids;
  proc_t proc_info;
  memset(&proc_info, 0, sizeof(proc_info));
  PROCTAB *proc = openproc(PROC_FILLMEM | PROC_FILLSTAT | PROC_FILLSTATUS);
  while (readproc(proc, &proc_info) != 0) {
    if (proc_info.ppid == ppid) {
      pids += to_string(proc_info.tgid) + "|";
    }
  }
  if (pids.back() == '|')
    pids.pop_back();
  pids += "\0";
  closeproc(proc);
  return pids;
}

} // namespace procinfo
