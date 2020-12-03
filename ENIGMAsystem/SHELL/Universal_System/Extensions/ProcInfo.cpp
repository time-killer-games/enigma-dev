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

#include "ProcInfo/procinfo.h"

using std::string;

#ifdef _WIN32
#define EXPORTED_FUNCTION extern "C" __declspec(dllexport)
#else /* macOS, Linux, and BSD */
#define EXPORTED_FUNCTION extern "C" __attribute__((visibility("default")))
#endif

EXPORTED_FUNCTION double process_execute(double ind, char *command) {
  return (double)procinfo::process_execute((unsigned long)ind, command);
}

EXPORTED_FUNCTION double process_execute_async(double ind, char *command) {
  procinfo::process_execute_async((unsigned long)ind, command);
  return 0;
}

EXPORTED_FUNCTION double process_current(double ind) {
  return (double)procinfo::process_current((unsigned long)ind);
}

EXPORTED_FUNCTION double process_previous(double ind) {
  return (double)procinfo::process_previous((unsigned long)ind);
}

EXPORTED_FUNCTION char *process_output(double ind) {
  static string result;
  result = procinfo::process_output((unsigned long)ind);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *process_evaluate(double ind) {
  static string result;
  result = procinfo::process_evaluate((unsigned long)ind);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double process_clear_pid(double ind) {
  procinfo::process_clear_pid((unsigned long)ind);
  return 0;
}

EXPORTED_FUNCTION double process_clear_out(double ind) {
  procinfo::process_clear_out((unsigned long)ind);
  return 0;
}

EXPORTED_FUNCTION double pid_from_self() {
  return procinfo::pid_from_self();
}

EXPORTED_FUNCTION double ppid_from_self() {
  return procinfo::ppid_from_self();
}

EXPORTED_FUNCTION char *path_from_pid(double pid) {
  static string result;
  result = procinfo::path_from_pid((unsigned long)pid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *dir_from_pid(double pid) {
  static string result;
  result = procinfo::dir_from_pid((unsigned long)pid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *name_from_pid(double pid) {
  static string result;
  result = procinfo::name_from_pid((unsigned long)pid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *cmd_from_pid(double pid) {
  static string result;
  result = procinfo::cmd_from_pid((unsigned long)pid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *env_from_pid(double pid) {
  static string result;
  result = procinfo::env_from_pid((unsigned long)pid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *env_from_pid_ext(double pid, char *name) {
  static string result;
  result = procinfo::env_from_pid_ext((unsigned long)pid, name);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *cwd_from_pid(double pid) {
  static string result;
  result = procinfo::cwd_from_pid((unsigned long)pid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double pid_exists(double pid) {
  return procinfo::pid_exists((unsigned long)pid);
}

EXPORTED_FUNCTION double wid_exists(char *wid) {
  return procinfo::wid_exists(wid);
}

EXPORTED_FUNCTION double pid_kill(double pid) {
  return procinfo::pid_kill((unsigned long)pid);
}

EXPORTED_FUNCTION void *window_from_wid(char *wid) {
  return reinterpret_cast<void *>(procinfo::window_from_wid(wid));
}

EXPORTED_FUNCTION char *wid_from_window(void *window) {
  static string result;
  result = procinfo::wid_from_window(reinterpret_cast<unsigned long long>(window));
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double pid_from_wid(char *wid) {
  return procinfo::pid_from_wid(wid);
}

EXPORTED_FUNCTION char *pids_enum(double trim_dir, double trim_empty) {
  static string result;
  result = procinfo::pids_enum((bool)trim_dir, (bool)trim_empty);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *pids_from_spec(char *name, double spec) {
  static string result;
  result = procinfo::pids_from_spec(name, (unsigned)spec);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double ppid_from_pid(double pid) {
  return procinfo::ppid_from_pid((unsigned long)pid);
}

EXPORTED_FUNCTION char *pids_from_ppid(double ppid) {
  static string result;
  result = procinfo::pids_from_ppid((unsigned long)ppid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *wids_from_pid(double pid) {
  static string result;
  result = procinfo::wids_from_pid((unsigned long)pid);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *wid_from_top() {
  static string result;
  result = procinfo::wid_from_top();
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double pid_from_top() {
  return procinfo::pid_from_top();
}

EXPORTED_FUNCTION double wid_to_top(char *wid) {
  procinfo::wid_to_top(wid);
  return 0;
}

EXPORTED_FUNCTION double wid_set_pwid(char *wid, char *pwid) {
  procinfo::wid_set_pwid(wid, pwid);
  return 0;
}

EXPORTED_FUNCTION char *echo(double ind, char *expression) {
  static string result;
  result = procinfo::echo(ind, expression);
  return (char *)result.c_str();
}
