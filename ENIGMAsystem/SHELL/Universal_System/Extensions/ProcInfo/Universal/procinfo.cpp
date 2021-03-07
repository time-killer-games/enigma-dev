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

#include <cstddef>
#include <algorithm>
#include <sstream>
#include <thread>

#include "../procinfo.h"

using std::string;
using std::vector;
using std::size_t;
using std::thread;

vector<string> string_split_by_first_equalssign(string str) {
  size_t pos = 0;
  vector<string> vec;
  if ((pos = str.find_first_of("=")) != string::npos) {
    vec.push_back(str.substr(0, pos));
    vec.push_back(str.substr(pos + 1));
  }
  return vec;
}

vector<string> string_split(string str, string delimiter) {
  vector<std::string> vec;
  std::stringstream sstr(str);
  string tmp;
  while (std::getline(sstr, tmp, delimiter[0]))
    vec.push_back(tmp);
  return vec;
}

namespace procinfo {

void process_execute_async(PROCID ind, string command) {
  thread proc_thread(process_execute, ind, command);
  proc_thread.detach();
}

std::string path_from_pid(PROCID pid) {
  char *exe = nullptr;
  XProc::ExeFromProcId(pid, &exe);
  return exe ? : "";
}

std::string cwd_from_pid(PROCID pid) {
  char *cwd = nullptr;
  XProc::CwdFromProcId(pid, &cwd);
  return cwd ? : "";
}
	
std::string cmd_from_pid(PROCID pid) {
  string result; char **cmdline; int size;
  XProc::CmdlineFromProcId(pid, &cmdline, &size);
  if (cmdline) {
    for (int i = 0; i < size; i++) {
      result += "\"" + string_replace_all(cmdline[i], "\"", "\\\"") + "\" ";
	}
	if (!result.empty()) {
      result.pop_back();
    }
	XProc::FreeCmdline(cmdline);
  }
  return result;
}

std::string env_from_pid(PROCID pid) {
  string result; char **environ; int size;
  XProc::EnvironFromProcId(pid, &environ, &size);
  if (environ) {
    for (int i = 0; i < size; i++) {
      vector<string> equalssplit = string_split_by_first_equalssign(environ[i]);
      for (int j = 0; j < equalssplit.size(); j++) {
        if (j == equalssplit.size() - 1) {
          equalssplit[j] = string_replace_all(equalssplit[j], "\"", "\\\"");
          result += equalssplit[0] + "=\"" + equalssplit[j] + "\"\n";
        }
      }
    }
    XProc::FreeEnviron(environ);
  }
  return result;
}

std::string env_from_pid_ext(PROCID pid, std::string name) {
  char *value;
  XProc::EnvironFromProcIdEx(pid, name.c_str(), &value);
  return value ? : "";
}

string dir_from_pid(PROCID pid) {
  string fname = path_from_pid(pid);
  size_t fp = fname.find_last_of("/\\");
  return fname.substr(0, fp + 1);
}

string name_from_pid(PROCID pid) {
  string fname = path_from_pid(pid);
  size_t fp = fname.find_last_of("/\\");
  return fname.substr(fp + 1);
}

PROCID pid_from_self() {
  PROCID procId;
  XProc::ProcIdFromSelf(&procId);
  return procId;
}

PROCID ppid_from_self() {
  PROCID parentProcId;
  XProc::ParentProcIdFromSelf(&parentProcId);
  return parentProcId;
}

PROCID ppid_from_pid(PROCID pid) {
  PROCID parentProcId;
  XProc::ParentProcIdFromProcId(pid, &parentProcId);
  return parentProcId;
}

std::string pids_from_ppid(PROCID ppid) {
  string result; PROCID *procId; int size;
  XProc::ProcIdFromParentProcId(ppid, &procId, &size);
  if (procId) {
    for (int i = 0; i < size; i++) {
      result += std::to_string(procId[i]) + "|";
	}
	if (!result.empty()) {
      result.pop_back();
    }
	free(procId);
  }
  return result;
}

std::string pids_enum(bool trim_dir, bool trim_empty) {
  PROCID *procId; int size;
  string result = "PID\tPPID\t";
  result += trim_dir ? "NAME\n" : "PATH\n";
  XProc::ProcIdEnumerate(&procId, &size);
  if (procId) {
    for (int i = 0; i < size; i++) {
      string exe = trim_dir ?
        name_from_pid(procId[i]) :
        path_from_pid(procId[i]);
      if (!trim_empty || !exe.empty()) {
        result += std::to_string(procId[i]) + "\t";
        result += std::to_string(ppid_from_pid(procId[i])) + "\t";
        result += exe + "\n";
      }
    }
    if (!result.empty()) {
      result.pop_back();
    }
	free(procId);
  }
  result += "\0";
  return result;
}

enum PIDRES_SPECTYPE {
  PIDRES_SPECNONE,
  PIDRES_SPECFILE,
  PIDRES_SPECPATH,
  PIDRES_SPECBOTH
};

std::string pids_from_spec(std::string name, int spec) {
  PROCID *procId; int size; string result;
  XProc::ProcIdEnumerate(&procId, &size);
  if (procId) {
    for (int i = 0; i < size; i++) {
      string exe;
      if (spec == PIDRES_SPECFILE) 
        exe = name_from_pid(procId[i]);
      if (spec == PIDRES_SPECPATH) 
        exe = dir_from_pid(procId[i]);
      if (spec == PIDRES_SPECBOTH) 
        exe = path_from_pid(procId[i]);
      if (name == exe || spec == PIDRES_SPECNONE)
        result += std::to_string(procId[i]) + "|";
    }
    if (!result.empty()) {
      result.pop_back();
    }
	free(procId);
  }
  result += "\0";
  return result;
}

bool pid_exists(PROCID pid) {
  return XProc::ProcIdExists(pid);
}

bool pid_kill(PROCID pid) {
  return XProc::ProcIdKill(pid);
}

} // namespace procinfo

