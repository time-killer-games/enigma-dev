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

#include "FileSystem/filesystem.h"

using std::string;

#ifdef _WIN32
#define EXPORTED_FUNCTION extern "C" __declspec(dllexport)
#else /* macOS, Linux, and BSD */
#define EXPORTED_FUNCTION extern "C" __attribute__((visibility("default")))
#endif

EXPORTED_FUNCTION char *get_working_directory() {
  static string result;
  result = filesystem::fs_get_working_directory();
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double set_working_directory(char *dname) {
  return filesystem::fs_set_working_directory(dname);
}

EXPORTED_FUNCTION char *get_temp_directory() {
  static string result;
  result = filesystem::fs_get_temp_directory();
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *get_program_directory() {
  static string result;
  result = filesystem::fs_get_program_directory();
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *get_program_filename() {
  static string result;
  result = filesystem::fs_get_program_filename();
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *get_program_pathname() {
  static string result;
  result = filesystem::fs_get_program_pathname();
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *filename_absolute(char *fname) {
  static string result;
  result = filesystem::fs_filename_absolute(fname);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *filename_canonical(char *fname) {
  static string result;
  result = filesystem::fs_filename_canonical(fname);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double file_exists(char *fname) {
  return filesystem::fs_file_exists(fname);
}

EXPORTED_FUNCTION double file_delete(char *fname) {
  return filesystem::fs_file_delete(fname);
}

EXPORTED_FUNCTION double file_rename(char *oldname, char *newname) {
  return filesystem::fs_file_rename(oldname, newname);
}

EXPORTED_FUNCTION double file_copy(char *fname, char *newname) {
  return filesystem::fs_file_copy(fname, newname);
}

EXPORTED_FUNCTION double file_size(char *fname) {
  return filesystem::fs_file_size(fname);
}

EXPORTED_FUNCTION double directory_exists(char *dname) {
  return filesystem::fs_directory_exists(dname);
}

EXPORTED_FUNCTION double directory_create(char *dname) {
  return filesystem::fs_directory_create(dname);
}

EXPORTED_FUNCTION double directory_destroy(char *dname) {
  return filesystem::fs_directory_destroy(dname);
}

EXPORTED_FUNCTION double directory_rename(char *oldname, char *newname) {
  return filesystem::fs_directory_rename(oldname, newname);
}

EXPORTED_FUNCTION double directory_copy(char *dname, char *newname) {
  return filesystem::fs_directory_copy(dname, newname);
}

EXPORTED_FUNCTION double directory_size(char *dname) {
  return filesystem::fs_directory_size(dname);
}

EXPORTED_FUNCTION char *directory_contents(char *dname) {
  static string result;
  result = filesystem::fs_directory_contents(dname);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *directory_contents_ext(char *dname, char *pattern, double includedirs) {
  static string result;
  result = filesystem::fs_directory_contents(dname, pattern, includedirs);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION char *environment_get_variable(char *name) {
  static string result;
  result = filesystem::fs_environment_get_variable(name);
  return (char *)result.c_str();
}

EXPORTED_FUNCTION double environment_set_variable(char *name, char *value) {
  return filesystem::fs_environment_set_variable(name, value);
}

EXPORTED_FUNCTION char *environment_expand_variables(char *str) {
  static string result;
  result = filesystem::fs_environment_expand_variables(str);
  return (char *)result.c_str();
}
