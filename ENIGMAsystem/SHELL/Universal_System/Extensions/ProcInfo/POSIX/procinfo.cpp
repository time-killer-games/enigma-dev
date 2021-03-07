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

#include <cstdio>
#include <mutex>
#include <map>

#include "../procinfo.h"

#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

using std::string;
using std::to_string;

namespace {

PROCID process_execute_helper(const char *command, int *infp, int *outfp) {
  int p_stdin[2];
  int p_stdout[2];
  PROCID pid;
  if (pipe(p_stdin) == -1)
    return -1;
  if (pipe(p_stdout) == -1) {
    close(p_stdin[0]);
    close(p_stdin[1]);
    return -1;
  }
  pid = fork();
  if (pid < 0) {
    close(p_stdin[0]);
    close(p_stdin[1]);
    close(p_stdout[0]);
    close(p_stdout[1]);
    return pid;
  } else if (pid == 0) {
    close(p_stdin[1]);
    dup2(p_stdin[0], 0);
    close(p_stdout[0]);
    dup2(p_stdout[1], 1);
    dup2(open("/dev/null", O_RDONLY), 2);
    for (int i = 3; i < 4096; i++)
      close(i);
    setsid();
    execl("/bin/sh", "/bin/sh", "-c", command, nullptr);
    exit(0);
  }
  close(p_stdin[0]);
  close(p_stdout[1]);
  if (infp == nullptr) {
    close(p_stdin[1]);
  } else {
    *infp = p_stdin[1];
  }
  if (outfp == nullptr) {
    close(p_stdout[0]);
  } else {
    *outfp = p_stdout[0];
  }
  return pid;
}

} // anonymous namespace

namespace procinfo {

static std::map<PROCID, PROCID> currpid;
static std::map<PROCID, PROCID> prevpid;

static std::mao<PROCID,    int> stdinpt;
static std::map<PROCID, string> currout;
static std::map<PROCID, string> prevout;

static std::mutex currout_mutex;

PROCID process_execute(PROCID ind, string command) {
  PROCID pid = 0; string output; char buffer[BUFSIZ];
  int infp  = 0, outfp = 0; ssize_t nRead = 0;
  pid = process_execute_helper(command.c_str(), &infp, &outfp);
  currpid.insert(std::make_pair(ind, pid));
  stdinpt.insert(std::make_pair(ind, infp));
  while ((nRead = read(outfp, buffer, BUFSIZ)) > 0) {
    buffer[nRead] = '\0';
    output.append(buffer, nRead);
    std::lock_guard<std::mutex> guard(currout_mutex);
    currout[ind] = output;
  }
  stdinpt.erase(ind);
  while (output.back() == '\r' || output.back() == '\n')
    output.pop_back();
  prevpid.insert(std::make_pair(ind, pid));
  prevout.insert(std::make_pair(ind, output));
  return pid;
}

PROCID process_current(PROCID ind) {
  if (currpid.find(ind) == currpid.end())
    return 0;
  PROCID pid = currpid.find(ind)->second;
  XProc::ParentProcIdFromProcIdSkipSh(pid, &pid);
  return pid;
}

PROCID process_previous(PROCID ind) {
  if (prevpid.find(ind) == prevpid.end())
    return 0;
  PROCID pid = prevpid.find(ind)->second;
  XProc::ParentProcIdFromProcIdSkipSh(pid, &pid);
  return pid;
}

void process_input(PROCID ind, string input) {
  if (stdinpt.find(ind) == stdinpt.end()) return; 
  char *buffer = new char[input.length() + 1]();
  strcpy(buffer, input.c_str());
  write(stdinpt[ind], buffer, input.length() + 1);
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

string echo(PROCID ind, string expression) {
  process_execute(ind, "echo " + expression);
  return process_evaluate(ind);
}

} // namepace procinfo
