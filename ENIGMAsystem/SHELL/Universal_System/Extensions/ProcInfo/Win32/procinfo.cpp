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

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <thread>
#include <mutex>
#include <map>

#include "../procinfo.h"

#define byte __windows_byte_workaround
#include <windows.h>
#undef byte

using std::string;
using std::to_string;
using std::wstring;
using std::vector;
using std::size_t;

static string proc_wids;
static PROCID proc_pid;
static inline BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lparam) {
  DWORD dw_pid;
  GetWindowThreadProcessId(hWnd, &dw_pid);
  if (dw_pid == proc_pid) {
    void *voidp_wid = reinterpret_cast<void *>(hWnd);
    proc_wids += to_string(reinterpret_cast<unsigned long long>(voidp_wid));
    proc_wids += "|";
  }
  return true;
}

namespace procinfo {

static std::map<PROCID, PROCID> currpid;
static std::map<PROCID, PROCID> prevpid;

static std::map<PROCID, HANDLE> stdinpt;
static std::map<PROCID, string> currout;
static std::map<PROCID, string> prevout;

static std::mutex currout_mutex;

static inline void output_thread(PROCID ind, HANDLE handle, string *output) {
  string result;
  DWORD dwRead = 0;
  char buffer[BUFSIZ];
  while (ReadFile(handle, buffer, BUFSIZ, &dwRead, nullptr) && dwRead) {
    buffer[dwRead] = '\0';
    result.append(buffer, dwRead);
    *(output) = result;
  }
}

PROCID process_execute(PROCID ind, string command) {
  PROCID pid = 0;
  string output;
  wstring wstr_command = widen(command);
  wchar_t cwstr_command[32768];
  wcsncpy_s(cwstr_command, 32768, wstr_command.c_str(), 32768);
  bool proceed = true;
  HANDLE hStdInPipeRead = nullptr;
  HANDLE hStdInPipeWrite = nullptr;
  HANDLE hStdOutPipeRead = nullptr;
  HANDLE hStdOutPipeWrite = nullptr;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, true };
  proceed = CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0);
  if (proceed == false) return pid;
  SetHandleInformation(hStdInPipeWrite, HANDLE_FLAG_INHERIT, 0);
  proceed = CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0);
  if (proceed == false) return pid;
  STARTUPINFOW si = { 0 };
  si.cb = sizeof(STARTUPINFOW);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdError = hStdOutPipeWrite;
  si.hStdOutput = hStdOutPipeWrite;
  si.hStdInput = hStdInPipeRead;
  PROCESS_INFORMATION pi = { 0 };
  if (CreateProcessW(nullptr, cwstr_command, nullptr, nullptr, true, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
    CloseHandle(hStdOutPipeWrite);
    CloseHandle(hStdInPipeRead);
    pid = pi.dwProcessId;
    currpid.insert(std::make_pair(ind, pid));
	stdinpt.insert(std::make_pair(ind, hStdInPipeWrite));
    MSG msg; HANDLE waitHandles[] = { pi.hProcess, hStdOutPipeRead };
    std::thread outthrd(output_thread, ind, hStdOutPipeRead, &output);
    while (MsgWaitForMultipleObjects(2, waitHandles, false, 5, QS_ALLEVENTS) != WAIT_OBJECT_0) {
      std::lock_guard<std::mutex> guard(currout_mutex);
      currout[ind] = output;
      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    outthrd.join();
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hStdOutPipeRead);
    CloseHandle(hStdInPipeWrite); 
    stdinpt.erase(ind);
    while (output.back() == '\r' || output.back() == '\n')
      output.pop_back();
    prevpid.insert(std::make_pair(ind, pid));
    prevout.insert(std::make_pair(ind, output));
  }
  return pid;
}

PROCID process_current(PROCID ind) {
  if (currpid.find(ind) == currpid.end())
    return 0;
  return currpid.find(ind)->second;
}

PROCID process_previous(PROCID ind) {
  if (prevpid.find(ind) == prevpid.end())
    return 0;
  return prevpid.find(ind)->second;
}

void process_input(PROCID ind, string input) {
  if (stdinpt.find(ind) == stdinpt.end()) return;
  char *buffer = new char[input.length() + 1]();
  strncpy_s(buffer, input.length() + 1, 
    input.c_str(), input.length() + 1);
  DWORD dwWritten; WriteFile(stdinpt[ind], buffer, 
    input.length() + 1, &dwWritten, nullptr);
  delete[] buffer;
}

string process_output(PROCID ind) {
  std::lock_guard<std::mutex> guard(currout_mutex);
  return currout[ind];
}

string process_evaluate(PROCID ind) {
  return prevout.find(ind)->second;
}

void process_clear_pid(PROCID ind) {
  currpid.erase(ind);
  prevpid.erase(ind);
}

void process_clear_out(PROCID ind) {
  std::lock_guard<std::mutex> guard(currout_mutex);
  currout.erase(ind);
  prevout.erase(ind);
}

bool wid_exists(wid_t wid) {
  void *voidp_wid = reinterpret_cast<void *>(stoull(wid, nullptr, 10));
  HWND hwnd_wid = reinterpret_cast<HWND>(voidp_wid);
  return (IsWindow(hwnd_wid) && hwnd_wid == GetAncestor(hwnd_wid, GA_ROOT));
}

window_t window_from_wid(wid_t wid) {
  return stoull(wid, nullptr, 10);
}

wid_t wid_from_window(window_t window) {
  return to_string(window);
}

PROCID pid_from_wid(wid_t wid) {
  DWORD dw_pid;
  void *voidp_wid = reinterpret_cast<void *>(stoull(wid, nullptr, 10));
  HWND hwnd_wid = reinterpret_cast<HWND>(voidp_wid);
  GetWindowThreadProcessId(hwnd_wid, &dw_pid);
  return dw_pid;
}

string wids_from_pid(PROCID pid) {
  proc_wids = "";
  proc_pid = pid;
  EnumWindows(EnumWindowsProc, 0);
  if (proc_wids.back() == '|')
    proc_wids.pop_back();
  return proc_wids;
}

wid_t wid_from_top() {
  void *voidp_wid = reinterpret_cast<void *>(GetForegroundWindow());
  return to_string(reinterpret_cast<unsigned long long>(voidp_wid));
}

PROCID pid_from_top() {
  return pid_from_wid(wid_from_top());
}

void wid_to_top(wid_t wid) {
  DWORD dw_pid; void *voidp_wid = reinterpret_cast<void *>(stoull(wid, nullptr, 10));
  HWND hwnd_wid = reinterpret_cast<HWND>(voidp_wid);
  GetWindowThreadProcessId(hwnd_wid, &dw_pid);
  AllowSetForegroundWindow(dw_pid);
  SetForegroundWindow(hwnd_wid);
}

void wid_set_pwid(wid_t wid, wid_t pwid) {
  void *voidp_wid = reinterpret_cast<void *>(stoull(wid, nullptr, 10));
  void *voidp_pwid = reinterpret_cast<void *>(stoull(pwid, nullptr, 10));
  HWND hwnd_wid = reinterpret_cast<HWND>(voidp_wid);
  LONG_PTR longp_pwid = reinterpret_cast<LONG_PTR>(voidp_pwid);
  SetWindowLongPtr(hwnd_wid, GWLP_HWNDPARENT, longp_pwid);
}

string echo(PROCID ind, string expression) {
  string result, command = "cmd /c @echo off & echo " + 
    StringReplaceAll(expression, "&", "^&");
  process_execute(ind, command);
  return process_evaluate(ind);
}

} // namespace procinfo
