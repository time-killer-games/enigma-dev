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

#include <string>
#include <vector>
#include <cstddef>

namespace strings {

  std::string string_replace_all(std::string str, std::string substr, std::string nstr);
  std::vector<std::string> string_split(std::string str, char delimiter);
  std::string filename_path(std::string fname);
  std::string filename_name(std::string fname);
  std::string filename_ext(std::string fname);
  std::string filename_remove_slash(std::string dname, bool canonical = false);
  std::string filename_add_slash(std::string dname, bool canonical = false);

} // namespace slashes

namespace filesystem {

  std::string fs_get_working_directory();
  bool fs_set_working_directory(std::string dname);
  std::string fs_get_temp_directory();
  std::string fs_get_program_directory();
  std::string fs_get_program_filename();
  std::string fs_get_program_pathname();
  std::string fs_filename_absolute(std::string fname);
  std::string fs_filename_canonical(std::string fname);
  bool fs_file_exists(std::string fname);
  bool fs_file_delete(std::string fname);
  bool fs_file_rename(std::string oldname, std::string newname);
  bool fs_file_copy(std::string fname, std::string newname);
  std::uintmax_t fs_file_size(std::string fname);
  bool fs_directory_exists(std::string dname);
  bool fs_directory_create(std::string dname);
  bool fs_directory_destroy(std::string dname);
  bool fs_directory_rename(std::string oldname, std::string newname);
  bool fs_directory_copy(std::string dname, std::string newname);
  std::uintmax_t fs_directory_size(std::string dname);
  std::string fs_directory_contents(std::string dname, std::string pattern = "*.*", bool includedirs = true);
  std::string fs_environment_get_variable(std::string name);
  bool fs_environment_set_variable(std::string name, std::string value);
  std::string fs_environment_expand_variables(std::string str);

} // namespace filesystem

namespace enigma_user {

  #ifdef OVERRIDE_SANDBOX
  using namespace filesystem;
  #define working_directory fs_get_working_directory()
  #define temp_directory fs_get_temp_directory()
  #define program_directory fs_get_program_directory()
  #define program_filename fs_get_program_filename()
  #define program_pathname fs_get_program_pathname()
  #define set_working_directory(x) fs_set_working_directory(x)
  #define filename_absolute(x) fs_filename_absolute(x)
  #define filename_canonical(x) fs_filename_canonical(x)
  #define file_exists(x) fs_file_exists(x)
  #define file_delete(x) fs_file_delete(x)
  #define file_rename(x, y) fs_file_rename(x, y)
  #define file_copy(x, y) fs_file_copy(x, y)
  #define file_size(x) fs_file_size(x)
  #define directory_exists(x) fs_directory_exists(x)
  #define directory_create(x) fs_directory_create(x)
  #define directory_destroy(x) fs_directory_destroy(x)
  #define directory_rename(x, y) fs_directory_rename(x, y)
  #define directory_copy(x, y) fs_directory_copy(x, y)
  #define directory_size(x) fs_directory_size(x)
  #define directory_contents(x) fs_directory_contents(x, "*.*", true)
  #define directory_contents_ext(x, y, z) fs_directory_contents(x, y, z)
  #define environment_get_variable(x) fs_environment_get_variable(x)
  #define environment_set_variable(x, y) fs_environment_set_variable(x, y)
  #define environment_expand_variables(x) fs_environment_expand_variables(x)
  #endif

} // namespace enigma_user
