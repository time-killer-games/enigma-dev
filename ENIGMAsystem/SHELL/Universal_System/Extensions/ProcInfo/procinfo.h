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

#include "Universal/xproc.h"
#include "Universal/helpers.h"

// individual platforms need their platform-specific
// window types casted to an unsigned long long type
#define window_t unsigned long long

// an unsigned long long window identifier placed in
// quotes for quick concatenation in shell scripting
#define wid_t std::string

std::vector<std::string> string_split_by_first_equalssign(std::string str);

namespace procinfo {

PROCID process_execute(PROCID ind, std::string command);
void process_execute_async(PROCID ind, std::string command);
PROCID process_current(PROCID ind);
PROCID process_previous(PROCID ind);
void process_input(PROCID ind, std::string input);
std::string process_output(PROCID ind);
std::string process_evaluate(PROCID ind);
void process_clear_pid(PROCID ind);
void process_clear_out(PROCID ind);
PROCID pid_from_self();
PROCID ppid_from_self();
std::string path_from_pid(PROCID pid);
std::string dir_from_pid(PROCID pid);
std::string name_from_pid(PROCID pid);
std::string cmd_from_pid(PROCID pid);
std::string env_from_pid(PROCID pid);
std::string env_from_pid_ext(PROCID pid, std::string name);
std::string cwd_from_pid(PROCID pid);
bool pid_exists(PROCID pid);
bool wid_exists(wid_t wid);
bool pid_kill(PROCID pid);
window_t window_from_wid(wid_t wid);
wid_t wid_from_window(window_t window);
PROCID pid_from_wid(wid_t wid);
std::string pids_enum(bool trim_dir, bool trim_empty);
std::string pids_from_spec(std::string name, int spec);
PROCID ppid_from_pid(PROCID pid);
std::string pids_from_ppid(PROCID ppid);
std::string wids_from_pid(PROCID pid);
wid_t wid_from_top();
PROCID pid_from_top();
void wid_to_top(wid_t wid);
void wid_set_pwid(wid_t wid, wid_t pwid);
std::string echo(PROCID ind, std::string expression);

} // namespace procinfo

namespace enigma_user {

using ::procinfo::process_execute;
using ::procinfo::process_execute_async;
using ::procinfo::process_current;
using ::procinfo::process_previous;
using ::procinfo::process_input;
using ::procinfo::process_output;
using ::procinfo::process_evaluate;
using ::procinfo::process_clear_pid;
using ::procinfo::process_clear_out;
using ::procinfo::pid_from_self;
using ::procinfo::ppid_from_self;
using ::procinfo::path_from_pid;
using ::procinfo::dir_from_pid;
using ::procinfo::name_from_pid;
using ::procinfo::cmd_from_pid;
using ::procinfo::env_from_pid;
using ::procinfo::env_from_pid_ext;
using ::procinfo::cwd_from_pid;
using ::procinfo::pid_exists;
using ::procinfo::wid_exists;
using ::procinfo::pid_kill;
using ::procinfo::window_from_wid;
using ::procinfo::wid_from_window;
using ::procinfo::pid_from_wid;
using ::procinfo::pids_enum;
using ::procinfo::pids_from_spec;
using ::procinfo::ppid_from_pid;
using ::procinfo::pids_from_ppid;
using ::procinfo::wids_from_pid;
using ::procinfo::wid_from_top;
using ::procinfo::pid_from_top;
using ::procinfo::wid_to_top;
using ::procinfo::wid_set_pwid;
using ::procinfo::echo;

} // namespace enigma_user

