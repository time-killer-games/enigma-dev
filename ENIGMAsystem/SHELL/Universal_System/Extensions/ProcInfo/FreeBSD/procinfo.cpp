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
#include <cstddef>
#include <cstdint>

#include "../procinfo.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/queue.h>
#include <sys/user.h>
#include <libprocstat.h>
#include <libutil.h>

using std::string;
using std::vector;
using std::to_string;
using std::size_t;

namespace procinfo {

string path_from_pid(process_t pid) {
  string path;
  size_t length;
  int mib[4];
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PATHNAME;
  mib[3] = pid;
  if (sysctl(mib, 4, NULL, &length, NULL, 0) == 0) {
    path.resize(length, '\0');
    char *buffer = path.data();
    if (sysctl(mib, 4, buffer, &length, NULL, 0) == 0) {
      path = string(buffer) + "\0";
    }
  }
  return path;
}

string cmd_from_pid(process_t pid) {
  string cmd;
  unsigned cntp;
  if (!pid_exists(pid)) { return ""; }
  struct procstat *proc_stat = procstat_open_sysctl();
  struct kinfo_proc *proc_info = procstat_getprocs(proc_stat, KERN_PROC_PID, pid, &cntp);
  char **args = procstat_getargv(proc_stat, proc_info, 0);
  if (args != NULL) {
    for (size_t i = 0; args[i] != NULL; i++) {
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
  procstat_freeargv(proc_stat);
  procstat_freeprocs(proc_stat, proc_info);
  procstat_close(proc_stat);
  return cmd;
}

string env_from_pid(process_t pid) {
  string env;
  unsigned cntp;
  if (!pid_exists(pid)) { return ""; }
  struct procstat *proc_stat = procstat_open_sysctl();
  struct kinfo_proc *proc_info = procstat_getprocs(proc_stat, KERN_PROC_PID, pid, &cntp);
  char **envs = procstat_getenvv(proc_stat, proc_info, 0);
  if (envs != NULL) {
    for (size_t i = 0; envs[i] != NULL; i++) {
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
  procstat_freeenvv(proc_stat);
  procstat_freeprocs(proc_stat, proc_info);
  procstat_close(proc_stat);
  return env;
}

string cwd_from_pid(process_t pid) {
  string cwd;
  unsigned cntp;
  if (!pid_exists(pid)) { return ""; }
  struct procstat *proc_stat = procstat_open_sysctl();
  struct kinfo_proc *proc_info = procstat_getprocs(proc_stat, KERN_PROC_PID, pid, &cntp);
  struct filestat_list *head = procstat_getfiles(proc_stat, proc_info, 0);
  struct filestat *fst;
  STAILQ_FOREACH(fst, head, next) {
    if (fst->fs_uflags & PS_FST_UFLAG_CDIR)
      cwd = fst->fs_path;
  }
  procstat_freefiles(proc_stat, head);
  procstat_freeprocs(proc_stat, proc_info);
  procstat_close(proc_stat);
  return (cwd.back() == '/' || cwd.empty()) ? cwd : cwd + "/";
}

string pids_enum(bool trim_dir, bool trim_empty) {
  int cntp;
  string pids = "PID\tPPID\t";
  pids += trim_dir ? "NAME\n" : "PATH\n";
  struct kinfo_proc *proc_info = kinfo_getallproc(&cntp);
  if (proc_info) {
    for (size_t i = 0; i < cntp; i++) {
      string exe = trim_dir ? 
        name_from_pid(proc_info[i].ki_pid) :
        path_from_pid(proc_info[i].ki_pid);
      if (!trim_empty || !exe.empty()) {
        pids += to_string(proc_info[i].ki_pid) + "\t";
        pids += to_string(proc_info[i].ki_ppid) + "\t";
        pids += exe + "\n";
      }
    }
  }
  if (pids.back() == '\n')
    pids.pop_back();
  pids += "\0";
  free(proc_info);
  return pids;
}

enum PIDRES_SPECTYPE {
  PIDRES_SPECNONE,
  PIDRES_SPECFILE,
  PIDRES_SPECPATH,
  PIDRES_SPECBOTH
};

string pids_from_spec(string name, unsigned spec) {
  string pids; int cntp;
  struct kinfo_proc *proc_info = kinfo_getallproc(&cntp);
  if (proc_info) {
    for (size_t i = 0; i < cntp; i++) {
      string exe;
      if (spec == PIDRES_SPECFILE) 
        exe = name_from_pid(proc_info[i].ki_pid);
      if (spec == PIDRES_SPECPATH) 
        exe = dir_from_pid(proc_info[i].ki_pid);
      if (spec == PIDRES_SPECBOTH) 
        exe = path_from_pid(proc_info[i].ki_pid);
      if (name == exe || spec == PIDRES_SPECNONE)
        pids += to_string(proc_info[i].ki_pid) + "|";
    }
  }
  if (pids.back() == '|')
    pids.pop_back();
  pids += "\0";
  free(proc_info);
  return pids;
}

process_t ppid_from_pid(process_t pid) {
  process_t ppid;
  struct kinfo_proc *proc_info = kinfo_getproc(pid);
  if (proc_info) {
    ppid = proc_info->ki_ppid;
  }
  free(proc_info);
  return ppid;
}

string pids_from_ppid(process_t ppid) {
  string pids; int cntp;
  struct kinfo_proc *proc_info = kinfo_getallproc(&cntp);
  if (proc_info) {
    for (size_t i = 0; i < cntp; i++) {
      if (proc_info[i].ki_ppid == ppid) {
        pids += to_string(proc_info[i].ki_pid) + "|";
      }
    }
  }
  if (pids.back() == '|')
    pids.pop_back();
  pids += "\0";
  free(proc_info);
  return pids;
}

} // namespace procinfo
