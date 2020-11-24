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

#include <filesystem>
#include <sstream>
#include <set>

#include "../filesystem.h"

namespace fs = std::filesystem;

using namespace strings;

using std::string;
using std::vector;
using std::size_t;

namespace strings {

  string string_replace_all(string str, string substr, string nstr) {
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != string::npos) {
      str.replace(pos, substr.length(), nstr);
      pos += nstr.length();
    }
    return str;
  }

  vector<string> string_split(string str, char delimiter) {
    vector<string> vec;
    std::stringstream sstr(str);
    string tmp;
    while (std::getline(sstr, tmp, delimiter))
      vec.push_back(tmp);
    return vec;
  }

  string filename_path(string fname) {
    size_t fp = fname.find_last_of("/\\");
    return fname.substr(0,fp + 1);
  }

  string filename_name(string fname) {
    size_t fp = fname.find_last_of("/\\");
    return fname.substr(fp + 1);
  }

  string filename_ext(string fname) {
    fname = filename_name(fname);
    size_t fp = fname.find_last_of(".");
    if (fp == string::npos)
      return "";
    return fname.substr(fp);
  }

} // namespace strings

namespace filesystem {

  string fs_get_working_directory() {
    std::error_code ec;
    string result = filename_add_slash(fs::current_path(ec).u8string());
    return (ec.value() == 0) ? result : "";
  }

  bool fs_set_working_directory(string dname) {
    std::error_code ec;
    const fs::path path = fs::u8path(dname);
    fs::current_path(path, ec);
    return (ec.value() == 0);
  }

  string fs_get_temp_directory() {
    std::error_code ec;
    string result = filename_add_slash(fs::temp_directory_path(ec).u8string());
    return (ec.value() == 0) ? result : "";
  }

  string fs_get_program_directory() {
    return filename_path(fs_get_program_pathname());
  }

  string fs_get_program_filename() {
    return filename_name(fs_get_program_pathname());
  }

  string fs_filename_absolute(string fname) {
    string result = "";
    if (fs_directory_exists(fname)) {
      result = filename_add_slash(fname, true);
    } else if (fs_file_exists(fname)) {
      result = fs_filename_canonical(fname);
    }
    return result;
  }
  
  string fs_filename_canonical(string fname) {
    std::error_code ec;
    fname = filesystem::fs_environment_expand_variables(fname);
    const fs::path path = fs::u8path(fname);
    string result = fs::weakly_canonical(path, ec).u8string();
    if (ec.value() == 0 && fs_directory_exists(result)) {
      return filename_add_slash(result);
    }
    return (ec.value() == 0) ? result : "";
  }

  std::uintmax_t fs_file_size(string fname) {
    std::error_code ec;
    if (!fs_file_exists(fname)) return 0;
    fname = filesystem::fs_environment_expand_variables(fname);
    const fs::path path = fs::u8path(fname);
    std::uintmax_t result = fs::file_size(path, ec);
    return (ec.value() == 0) ? result : 0;
  }

  bool fs_file_exists(string fname) {
    std::error_code ec;
    fname = filesystem::fs_environment_expand_variables(fname);
    const fs::path path = fs::u8path(fname);
    return (fs::exists(path, ec) && ec.value() == 0 && 
      (!fs::is_directory(path, ec)) && ec.value() == 0);
  }

  bool fs_file_delete(string fname) {
    std::error_code ec;
    if (!fs_file_exists(fname)) return false;
    fname = filesystem::fs_environment_expand_variables(fname);
    const fs::path path = fs::u8path(fname);
    return (fs::remove(path, ec) && ec.value() == 0);
  }

  bool fs_file_rename(string oldname, string newname) {
    std::error_code ec;
    if (!fs_file_exists(oldname)) return false;
    oldname = filesystem::fs_environment_expand_variables(oldname);
    newname = filesystem::fs_environment_expand_variables(newname);
    if (!fs_directory_exists(filename_path(newname)))
      fs_directory_create(filename_path(newname));
    const fs::path path1 = fs::u8path(oldname);
    const fs::path path2 = fs::u8path(newname);
    fs::rename(path1, path2, ec);
    return (ec.value() == 0);
  }

  bool fs_file_copy(string fname, string newname) {
    std::error_code ec;
    if (!fs_file_exists(fname)) return false;
    fname = filesystem::fs_environment_expand_variables(fname);
    newname = filesystem::fs_environment_expand_variables(newname);
    if (!fs_directory_exists(filename_path(newname)))
      fs_directory_create(filename_path(newname));
    const fs::path path1 = fs::u8path(fname);
    const fs::path path2 = fs::u8path(newname);
    fs::copy(path1, path2, ec);
    return (ec.value() == 0);
  }

  std::uintmax_t fs_directory_size(string dname) {
    std::uintmax_t result = 0;
    if (!fs_directory_exists(dname)) return 0;
    const fs::path path = fs::u8path(filename_remove_slash(dname, true));
    if (fs::exists(path)) {
      fs::directory_iterator end_itr;
      for (fs::directory_iterator dir_ite(path); dir_ite != end_itr; dir_ite++) {
        fs::path file_path = fs::u8path(fs_filename_absolute(dir_ite->path().u8string()));
        if (!fs::is_directory(dir_ite->status())) {
          result += fs_file_size(file_path.u8string());
        } else {
          result += fs_directory_size(file_path.u8string());
        }
      }
    }
    return result;
  }

  bool fs_directory_exists(string dname) {
    std::error_code ec;
    dname = filename_remove_slash(dname, false);
    dname = fs_environment_expand_variables(dname);
    const fs::path path = fs::u8path(dname);
    return (fs::exists(path, ec) && ec.value() == 0 && 
      fs::is_directory(path, ec) && ec.value() == 0);
  }

  bool fs_directory_create(string dname) {
    std::error_code ec;
    dname = filename_remove_slash(dname, true);
    const fs::path path = fs::u8path(dname);
    return (fs::create_directories(path, ec) && ec.value() == 0);
  }

  bool fs_directory_destroy(string dname) {
    std::error_code ec;
    if (!fs_directory_exists(dname)) return false;
    dname = filename_remove_slash(dname, true);
    const fs::path path = fs::u8path(dname);
    return (fs::remove_all(path, ec) && ec.value() == 0);
  }

  bool fs_directory_rename(string oldname, string newname) {
    std::error_code ec;
    if (!fs_directory_exists(oldname)) return false;
    if (!fs_directory_exists(newname)) fs_directory_create(newname);
    oldname = filename_remove_slash(oldname, true);
    newname = filename_remove_slash(newname, true);
    bool result = false;
    const fs::path path1 = fs::u8path(oldname);
    const fs::path path2 = fs::u8path(newname);
    const fs::path path3 = fs::u8path(path2.u8string().substr(0, path1.u8string().length()));
    if (fs_directory_exists(oldname)) {
      if ((filename_name(path1.u8string()) != filename_name(path2.u8string()) &&
        filename_path(path1.u8string()) == filename_path(path2.u8string())) ||
        path1.u8string() != path3.u8string()) {
        fs::rename(path1, path2, ec);
        result = (ec.value() == 0);
      }
    }
    return result;
  }

  static string retained_string = "";
  static size_t retained_length = 0;
  static std::uintmax_t szSrc   = 0;
  // this function was written to prevent infinitely copying inside itself
  static inline bool directory_copy_retained(string dname, string newname) {
    std::error_code ec;
    bool result = false;
    const fs::path path1 = fs::u8path(dname);
    const fs::path path2 = fs::u8path(newname);
    const fs::path path3 = fs::u8path(path2.u8string().substr(0, path1.u8string().length()));
    if (retained_string.empty() && retained_length == 0) {
      retained_length = path1.u8string().length();
      retained_string = path2.u8string().substr(retained_length);
    }
    if (fs_directory_exists(dname)) {
      if ((filename_name(path1.u8string()) != filename_name(path2.u8string()) &&
        filename_path(path1.u8string()) == filename_path(path2.u8string())) ||
        path1.u8string() != path3.u8string()) {
        fs::copy(path1, path2, fs::copy_options::recursive, ec);
        result = (ec.value() == 0);
      } else if (path1.u8string() == path3.u8string()) {
        vector<string> itemVec = string_split(fs_directory_contents(dname, "*.*", true), '\n');
        if (!fs_directory_exists(newname)) {
          fs_directory_create(newname);
          for (const string &item : itemVec) {
            if (fs_directory_exists(filename_remove_slash(item)) && 
              filename_remove_slash(item).substr(retained_length) != retained_string) {
              directory_copy_retained(filename_remove_slash(item), filename_add_slash(path2.u8string()) + 
              filename_name(filename_remove_slash(item)));
            } else if (fs_file_exists(item)) {
              fs::copy(item, filename_add_slash(path2.u8string()) + filename_name(item), ec);
              // ignore and skip errored copies and copy what is left.
              // uncomment the line below to break if one copy failed.
              // if (ec.value() == 0) { result = true; } else { return false; }
            }
          }
          // check size to determine success instead of error code.
          // comment the line below out if you want break on error.
          result = (fs_directory_exists(newname) && szSrc == fs_directory_size(newname));
        }
      }
    }
    return result;
  }

  bool fs_directory_copy(string dname, string newname) {
    if (!fs_directory_exists(dname)) return false;
    dname = filename_remove_slash(dname, true);
    newname = filename_remove_slash(newname, true);
    retained_string = "";
    retained_length = 0;
    // check size to determine success instead of error code.
    // comment the line below out if you want break on error.
    szSrc = fs_directory_size(dname);
    return directory_copy_retained(dname, newname);
  }

  string fs_directory_contents(string dname, string pattern, bool includedirs) {
    std::error_code ec;
    string result = "";
    if (!fs_directory_exists(dname)) return "";
    dname = filename_remove_slash(dname, true);
    const fs::path path = fs::u8path(dname);
    if (fs_directory_exists(dname)) {
      fs::directory_iterator end_itr;
      for (fs::directory_iterator dir_ite(path, ec); dir_ite != end_itr; dir_ite++) {
        if (ec.value() != 0) { break; }
        fs::path file_path = fs::u8path(fs_filename_absolute(dir_ite->path().u8string()));
        if (!fs::is_directory(dir_ite->status(ec)) && ec.value() == 0) {
          result += file_path.u8string() + "\n";
        } else if (ec.value() == 0 && includedirs) {
          result += filename_add_slash(file_path.u8string()) + "\n";
        }
      }
    }
    if (pattern.empty()) pattern = "*.*";
    if (result.back() == '\n') result.pop_back();
    pattern = string_replace_all(pattern, " ", "");
    pattern = string_replace_all(pattern, "*", "");
    vector<string> itemVec = string_split(result, '\n');
    vector<string> extVec = string_split(pattern, ';');
    std::set<string> filteredItems;
    for (const string &item : itemVec) {
      for (const string &ext : extVec) {
        if (ext == "." || ext == filename_ext(item) || fs_directory_exists(item)) {
          filteredItems.insert(item);
          break;
        }
      }
    }
    result = "";
    if (filteredItems.empty()) return result;
    for (const string &filteredName : filteredItems) {
      result += filteredName + "\n";
    }
    result.pop_back();
    return result;
  }

  string fs_environment_expand_variables(string str) {
    if (str.find("${") == string::npos) return str;
    string pre = str.substr(0, str.find("${"));
    string post = str.substr(str.find("${") + 2);
    if (post.find('}') == string::npos) return str;
    string variable = post.substr(0, post.find('}'));
    post = post.substr(post.find('}') + 1);
    string value = fs_environment_get_variable(variable);
    return fs_environment_expand_variables(pre + value + post);
  }

} // namespace filesystem
