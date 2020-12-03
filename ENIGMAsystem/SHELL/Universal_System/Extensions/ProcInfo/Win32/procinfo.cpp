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

#define _WIN32_DCOM

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cwchar>
#include <thread>
#include <mutex>
#include <map>

#ifdef EXE_INCLUDES
#include <fstream>
#include <cstdio>
#endif

#include "../procinfo.h"

#ifdef EXE_INCLUDES
#include "procinfo.h"
#endif

#include <wbemidl.h>

#define byte __windows_byte_workaround
#include <windows.h>
#undef byte

#include <Objbase.h>
#include <tlhelp32.h>
#include <winternl.h>
#include <psapi.h>

#ifdef _MSC_VER
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "wbemuuid.lib")
#endif

using std::string;
using std::to_string;
using std::wstring;
using std::vector;
using std::size_t;

#define RTL_DRIVE_LETTER_CURDIR struct {\
  WORD Flags;\
  WORD Length;\
  ULONG TimeStamp;\
  STRING DosPath;\
}

#define RTL_USER_PROCESS_PARAMETERS struct {\
  ULONG MaximumLength;\
  ULONG Length;\
  ULONG Flags;\
  ULONG DebugFlags;\
  PVOID ConsoleHandle;\
  ULONG ConsoleFlags;\
  PVOID StdInputHandle;\
  PVOID StdOutputHandle;\
  PVOID StdErrorHandle;\
  UNICODE_STRING CurrentDirectoryPath;\
  PVOID CurrentDirectoryHandle;\
  UNICODE_STRING DllPath;\
  UNICODE_STRING ImagePathName;\
  UNICODE_STRING CommandLine;\
  PVOID Environment;\
  ULONG StartingPositionLeft;\
  ULONG StartingPositionTop;\
  ULONG Width;\
  ULONG Height;\
  ULONG CharWidth;\
  ULONG CharHeight;\
  ULONG ConsoleTextAttributes;\
  ULONG WindowFlags;\
  ULONG ShowWindowFlags;\
  UNICODE_STRING WindowTitle;\
  UNICODE_STRING DesktopName;\
  UNICODE_STRING ShellInfo;\
  UNICODE_STRING RuntimeData;\
  RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[32];\
  ULONG EnvironmentSize;\
}

#ifdef PROCINFO_SELF_CONTAINED
static inline wstring widen(string str) {
  size_t wchar_count = str.size() + 1;
  vector<wchar_t> buf(wchar_count);
  return wstring { buf.data(), (size_t)MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buf.data(), (int)wchar_count) };
}

static inline string shorten(wstring wstr) {
  int nbytes = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), NULL, 0, NULL, NULL);
  vector<char> buf(nbytes);
  return string { buf.data(), (size_t)WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), buf.data(), nbytes, NULL, NULL) };
}
#endif

static inline HANDLE OpenProcessWithDebugPrivilege(process_t pid) {
  HANDLE hToken;
  LUID luid;
  TOKEN_PRIVILEGES tkp;
  OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
  LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
  tkp.PrivilegeCount = 1;
  tkp.Privileges[0].Luid = luid;
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);
  CloseHandle(hToken);
  return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

static inline bool IsX86Process(HANDLE process) {
  BOOL isWow = true;
  SYSTEM_INFO systemInfo = { 0 };
  GetNativeSystemInfo(&systemInfo);
  if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
    return isWow;
  IsWow64Process(process, &isWow);
  return isWow;
}

static inline wchar_t *GetEnvOrCwdW(HANDLE proc, bool env) {
  PEB peb;
  SIZE_T nRead;
  ULONG res_len = 0;
  PROCESS_BASIC_INFORMATION pbi;
  RTL_USER_PROCESS_PARAMETERS upp;
  HMODULE p_ntdll = GetModuleHandleW(L"ntdll.dll");
  typedef NTSTATUS (__stdcall *tfn_qip)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
  tfn_qip pfn_qip = tfn_qip(GetProcAddress(p_ntdll, "NtQueryInformationProcess"));
  NTSTATUS status = pfn_qip(proc, ProcessBasicInformation, &pbi, sizeof(pbi), &res_len);
  if (status) { return NULL; } 
  ReadProcessMemory(proc, pbi.PebBaseAddress, &peb, sizeof(peb), &nRead);
  if (!nRead) { return NULL; }
  ReadProcessMemory(proc, peb.ProcessParameters, &upp, sizeof(upp), &nRead);
  if (!nRead) { return NULL; }
  PVOID buffer = NULL;
  ULONG length = 0;
  if (env) {
    buffer = upp.Environment;
    length = upp.EnvironmentSize;
  } else {
    buffer = upp.CurrentDirectoryPath.Buffer;
    length = upp.CurrentDirectoryPath.Length;
  }
  wchar_t *res = new wchar_t[length / 2 + 1];
  ReadProcessMemory(proc, buffer, res, length, &nRead);
  if (!nRead) { return NULL; }
  res[length / 2] = 0;
  return res;
}

static string proc_wids;
static process_t proc_pid;
static inline BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lparam) {
  DWORD dw_pid;
  GetWindowThreadProcessId(hWnd, &dw_pid);
  if (dw_pid == proc_pid) {
    void *voidp_wid = reinterpret_cast<void *>(hWnd);
    proc_wids += to_string(reinterpret_cast<unsigned long long>(voidp_wid));
    proc_wids += "|";
  }
  return TRUE;
}

namespace procinfo {

static std::map<process_t, process_t> currpid;
static std::map<process_t, process_t> prevpid;

static std::map<process_t, string> currout;
static std::map<process_t, string> prevout;

std::mutex currout_mutex;

static inline void output_thread(process_t ind, HANDLE handle, string *output) {
  string result;
  DWORD dwRead = 0;
  char buffer[BUFSIZ];
  while (ReadFile(handle, buffer, BUFSIZ, &dwRead, NULL) && dwRead) {
    buffer[dwRead] = '\0';
    result.append(buffer, dwRead);
    *(output) = result;
  }
}

process_t process_execute(process_t ind, string command) {
  DWORD pid = 0;
  string output;
  wstring wstr_command = widen(command);
  wchar_t cwstr_command[32768];
  wcsncpy_s(cwstr_command, 32768, wstr_command.c_str(), 32768);
  BOOL proceed = TRUE;
  HANDLE hStdInPipeRead = NULL;
  HANDLE hStdInPipeWrite = NULL;
  HANDLE hStdOutPipeRead = NULL;
  HANDLE hStdOutPipeWrite = NULL;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  proceed = CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0);
  if (proceed == FALSE) return pid;
  proceed = CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0);
  if (proceed == FALSE) return pid;
  STARTUPINFOW si = { 0 };
  si.cb = sizeof(STARTUPINFOW);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdError = hStdOutPipeWrite;
  si.hStdOutput = hStdOutPipeWrite;
  si.hStdInput = hStdInPipeRead;
  PROCESS_INFORMATION pi = { 0 };
  if (CreateProcessW(NULL, cwstr_command, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
    pid = pi.dwProcessId;
    currpid.insert(std::make_pair(ind, pid));
    CloseHandle(hStdOutPipeWrite);
    CloseHandle(hStdInPipeRead);
    MSG msg;
    HANDLE waitHandles[] = { pi.hProcess, hStdOutPipeRead };
    std::thread outthrd(output_thread, ind, hStdOutPipeRead, &output);
    while (MsgWaitForMultipleObjects(2, waitHandles, false, 5, QS_ALLEVENTS) != WAIT_OBJECT_0) {
      std::lock_guard<std::mutex> guard(currout_mutex);
      currout[ind] = output;
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }
    outthrd.join();
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hStdOutPipeRead);
    CloseHandle(hStdInPipeWrite);
    while (output.back() == '\r' || output.back() == '\n')
      output.pop_back();
    prevpid.insert(std::make_pair(ind, pid));
    prevout.insert(std::make_pair(ind, output));
  }
  return pid;
}

process_t process_current(process_t ind) {
  if (currpid.find(ind) == currpid.end())
    return 0;
  return currpid.find(ind)->second;
}

process_t process_previous(process_t ind) {
  if (prevpid.find(ind) == prevpid.end())
    return 0;
  return prevpid.find(ind)->second;
}

string process_output(process_t ind) {
  std::lock_guard<std::mutex> guard(currout_mutex);
  return currout[ind];
}

string process_evaluate(process_t ind) {
  return prevout.find(ind)->second;
}

void process_clear_pid(process_t ind) {
  currpid.erase(ind);
  prevpid.erase(ind);
}

void process_clear_out(process_t ind) {
  std::lock_guard<std::mutex> guard(currout_mutex);
  currout.erase(ind);
  prevout.erase(ind);
}

process_t pid_from_self() {
  return GetCurrentProcessId();
}

process_t ppid_from_self() {
  return ppid_from_pid(pid_from_self());
}

string path_from_pid(process_t pid) {
  string path;
  HANDLE hProcess = OpenProcessWithDebugPrivilege(pid);
  wchar_t szFilename[MAX_PATH]; DWORD dwPathSize = MAX_PATH;
  if (QueryFullProcessImageNameW(hProcess, 0, szFilename, &dwPathSize) != 0) {
    path = shorten(szFilename);
  }
  CloseHandle(hProcess);
  return path;
}

string cmd_from_pid(process_t pid) {
  string cmd;
  HRESULT hr = 0;
  IWbemLocator *WbemLocator  = NULL;
  IWbemServices *WbemServices = NULL;
  IEnumWbemClassObject *EnumWbem  = NULL;
  hr = CoInitializeEx(0, COINIT_MULTITHREADED);
  hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
  hr = CoCreateInstance((REFCLSID)CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&WbemLocator);
  hr = WbemLocator->ConnectServer((BSTR)L"ROOT\\CIMV2", NULL, NULL, NULL, 0, NULL, NULL, &WbemServices);   
  hr = WbemServices->ExecQuery((BSTR)L"WQL", (BSTR)L"SELECT ProcessId,CommandLine FROM Win32_Process", WBEM_FLAG_FORWARD_ONLY, NULL, &EnumWbem);
  if (EnumWbem != NULL) {
    IWbemClassObject *result = NULL;
    ULONG returnedCount = 0;
    while ((hr = EnumWbem->Next(WBEM_INFINITE, 1, &result, &returnedCount)) == S_OK) {
      VARIANT ProcessId;
      VARIANT CommandLine;
      hr = result->Get(L"ProcessId", 0, &ProcessId, 0, 0);
      hr = result->Get(L"CommandLine", 0, &CommandLine, 0, 0);            
      if (ProcessId.uintVal == pid && !(CommandLine.vt == VT_NULL))
      cmd = shorten((wchar_t *)CommandLine.bstrVal);
      result->Release();
    }
  }
  EnumWbem->Release();
  WbemServices->Release();
  WbemLocator->Release();
  CoUninitialize();
  return cmd;
}

string env_from_pid(process_t pid) {
  #ifdef EXE_INCLUDES
  HANDLE proc = OpenProcessWithDebugPrivilege(pid);
  string exe;
  if (IsX86Process(proc)) {
    exe = echo(ULONG_MAX, "%TMP%") + "\\プロセス情報を取得する32.exe";
  } else {
    exe = echo(ULONG_MAX, "%TMP%") + "\\プロセス情報を取得する64.exe";
  }
  wstring wexe = widen(exe);
  FILE *file = NULL;
  std::wofstream strm(wexe.c_str());
  strm.close();
  if (_wfopen_s(&file, wexe.c_str(), L"wb") == 0) {
    if (IsX86Process(proc)) {
      fwrite((char *)getprocinfo32, sizeof(char), sizeof(getprocinfo32), file);
    } else {
      fwrite((char *)getprocinfo64, sizeof(char), sizeof(getprocinfo64), file);
    }
    fclose(file);
  }
  CloseHandle(proc);
  process_clear_out(ULONG_MAX);
  process_clear_pid(ULONG_MAX);
  process_execute(ULONG_MAX, "\"" + exe + "\" --get-env " + std::to_string(pid));
  string result = process_evaluate(ULONG_MAX);
  process_clear_out(ULONG_MAX);
  process_clear_pid(ULONG_MAX);
  return result;
  #else
  string envs;
  if (pid == 0 || !pid_exists(pid)) { return ""; }
  HANDLE proc = OpenProcessWithDebugPrivilege(pid);
  wchar_t *wenvs = NULL;
  if (IsX86Process(GetCurrentProcess())) {
    if (IsX86Process(proc)) {
      wenvs = GetEnvOrCwdW(proc, true);
    }
  } else {
    if (!IsX86Process(proc)) {
      wenvs = GetEnvOrCwdW(proc, true);
    }
  }
  string arg;
  if (wenvs == NULL) { 
    return ""; 
  } else {
    arg = shorten(wenvs);
  }
  size_t i = 0;
  do {
    size_t j = 0;
    vector<string> envVec = string_split_by_first_equalssign(arg);
    for (const string &env : envVec) {
      if (j == 0) { 
        if (env.find_first_of("%<>^&|:") != string::npos) { continue; }
        if (env.empty()) { continue; }
        envs += env; 
      } else { envs += "=\"" + string_replace_all(env, "\"", "\\\"") + "\"\n"; }
      j++;
    }
    i += wcslen(wenvs + i) + 1;
    arg = shorten(wenvs + i);
  } while (wenvs[i] != L'\0');
  if (envs.back() == '\n') { envs.pop_back(); }
  if (wenvs != NULL) { delete[] wenvs; } 
  CloseHandle(proc);
  return envs;
  #endif
}

string cwd_from_pid(process_t pid) {
  #ifdef EXE_INCLUDES
  HANDLE proc = OpenProcessWithDebugPrivilege(pid);
  string exe;
  if (IsX86Process(proc)) {
    exe = echo(ULONG_MAX, "%TMP%") + "\\プロセス情報を取得する32.exe";
  } else {
    exe = echo(ULONG_MAX, "%TMP%") + "\\プロセス情報を取得する64.exe";
  }
  wstring wexe = widen(exe);
  FILE *file = NULL;
  std::wofstream strm(wexe.c_str());
  strm.close();
  if (_wfopen_s(&file, wexe.c_str(), L"wb") == 0) {
    if (IsX86Process(proc)) {
      fwrite((char *)getprocinfo32, sizeof(char), sizeof(getprocinfo32), file);
    } else {
      fwrite((char *)getprocinfo64, sizeof(char), sizeof(getprocinfo64), file);
    }
    fclose(file);
  }
  CloseHandle(proc);
  process_clear_out(ULONG_MAX);
  process_clear_pid(ULONG_MAX);
  process_execute(ULONG_MAX, "\"" + exe + "\" --get-cwd " + std::to_string(pid));
  string result = process_evaluate(ULONG_MAX);
  process_clear_out(ULONG_MAX);
  process_clear_pid(ULONG_MAX);
  return result;
  #else
  string cwd;
  HANDLE proc = OpenProcessWithDebugPrivilege(pid);
  wchar_t *wcwd = NULL;
  if (IsX86Process(GetCurrentProcess())) {
    if (IsX86Process(proc)) {
      wcwd = GetEnvOrCwdW(proc, false);
    }
  } else {
    if (!IsX86Process(proc)) {
      wcwd = GetEnvOrCwdW(proc, false);
    }
  }
  if (wcwd == NULL) { 
    return ""; 
  } else {
    cwd = shorten(wcwd);
    delete[] wcwd;
  } 
  CloseHandle(proc);
  return (cwd.back() == '\\' || cwd.empty()) ? cwd : cwd + "\\";
  #endif
}

bool pid_exists(process_t pid) {
  // Slower than OpenProcess + GetExitCodeProcess approach,
  // but doesn't return true with processes of a wrong pid.
  // OpenProcess will succeed with a specific number within
  // 3 of any existing pid, which returns true incorrectly.
  bool result = false;
  HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 pe = { 0 };
  pe.dwSize = sizeof(PROCESSENTRY32);
  if (Process32First(hp, &pe)) {
    do {
      if (pe.th32ProcessID == pid) {
        result = true;
        break;
      }
    } while (Process32Next(hp, &pe));
  }
  CloseHandle(hp);
  return result;
}

bool wid_exists(wid_t wid) {
  void *voidp_wid = reinterpret_cast<void *>(stoull(wid, nullptr, 10));
  HWND hwnd_wid = reinterpret_cast<HWND>(voidp_wid);
  return (IsWindow(hwnd_wid) && hwnd_wid == GetAncestor(hwnd_wid, GA_ROOT));
}

bool pid_kill(process_t pid) {
  HANDLE hProcess = OpenProcessWithDebugPrivilege(pid);
  if (hProcess == NULL) return false;
  bool result = TerminateProcess(hProcess, 0);
  CloseHandle(hProcess);
  return result;
}

window_t window_from_wid(wid_t wid) {
  return stoull(wid, nullptr, 10);
}

wid_t wid_from_window(window_t window) {
  return to_string(window);
}

process_t pid_from_wid(wid_t wid) {
  DWORD dw_pid;
  void *voidp_wid = reinterpret_cast<void *>(stoull(wid, nullptr, 10));
  HWND hwnd_wid = reinterpret_cast<HWND>(voidp_wid);
  GetWindowThreadProcessId(hwnd_wid, &dw_pid);
  return dw_pid;
}

string pids_enum(bool trim_dir, bool trim_empty) {
  string pids = "PID\tPPID\t";
  pids += trim_dir ? "NAME\n" : "PATH\n";
  HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 pe = { 0 };
  pe.dwSize = sizeof(PROCESSENTRY32);
  if (Process32First(hp, &pe)) {
    do {
      string exe = trim_dir ? 
        name_from_pid(pe.th32ProcessID) :
        path_from_pid(pe.th32ProcessID);
      if (!trim_empty || !exe.empty()) {
        pids += to_string(pe.th32ProcessID) + "\t";
        pids += to_string(pe.th32ParentProcessID) + "\t";
        pids += exe + "\n";
      }
    } while (Process32Next(hp, &pe));
  }
  if (pids.back() == '\n')
    pids.pop_back();
  pids += "\0";
  CloseHandle(hp);
  return pids;
}

enum PIDRES_SPECTYPE {
  PIDRES_SPECNONE,
  PIDRES_SPECFILE,
  PIDRES_SPECPATH,
  PIDRES_SPECBOTH
};

string pids_from_spec(string name, unsigned spec) {
  string pids;
  HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 pe = { 0 };
  pe.dwSize = sizeof(PROCESSENTRY32);
  if (Process32First(hp, &pe)) {
    do {
      string exe;
      if (spec == PIDRES_SPECFILE) 
        exe = name_from_pid(pe.th32ProcessID);
      if (spec == PIDRES_SPECPATH) 
        exe = dir_from_pid(pe.th32ProcessID);
      if (spec == PIDRES_SPECBOTH) 
        exe = path_from_pid(pe.th32ProcessID);
      if (name == exe || spec == PIDRES_SPECNONE)
        pids += to_string(pe.th32ProcessID) + "|";
    } while (Process32Next(hp, &pe));
  }
  if (pids.back() == '|')
    pids.pop_back();
  pids += "\0";
  CloseHandle(hp);
  return pids;
}

process_t ppid_from_pid(process_t pid) {
  process_t ppid;
  HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 pe = { 0 };
  pe.dwSize = sizeof(PROCESSENTRY32);
  if (Process32First(hp, &pe)) {
    do {
      if (pe.th32ProcessID == pid) {
        ppid = pe.th32ParentProcessID;
        break;
      }
    } while (Process32Next(hp, &pe));
  }
  CloseHandle(hp);
  return ppid;
}

string pids_from_ppid(process_t ppid) {
  string pids;
  HANDLE hp = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 pe = { 0 };
  pe.dwSize = sizeof(PROCESSENTRY32);
  if (Process32First(hp, &pe)) {
    do {
      if (pe.th32ParentProcessID == ppid) {
        pids += to_string(pe.th32ProcessID) + "|";
      }
    } while (Process32Next(hp, &pe));
  }
  if (pids.back() == '|')
    pids.pop_back();
  pids += "\0";
  CloseHandle(hp);
  return pids;
}

string wids_from_pid(process_t pid) {
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

process_t pid_from_top() {
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

string echo(process_t ind, string expression) {
  string result, command = "cmd /c @echo off & echo " + 
    string_replace_all(expression, "&", "^&");
  process_execute(ind, command);
  return process_evaluate(ind);
}

} // namespace procinfo
