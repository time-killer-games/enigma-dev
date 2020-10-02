#include <sstream>

#include "../procinfo.h"

using std::string;
using std::to_string;
using std::stringstream;

using procinfo::cwd_from_pid;
using procinfo::env_from_pid;

static inline string string_digits(string str) {
  string result;
  stringstream ss;
  ss << str;
  string temp;
  unsigned long found; 
  while (!ss.eof()) { 
    ss >> temp; 
    if (stringstream(temp) >> found) 
      result += to_string(found); 
    temp = ""; 
  }
  return result;
} 

int main(int argc, char **argv) {
  if (argc == 3 && string(argv[1]) == "--get-cwd" && !string_digits(argv[2]).empty()) {
    printf("%s", cwd_from_pid(stoul(string_digits(argv[2]), nullptr, 10)).c_str());
    printf("%s", "\r\n");
  } else if (argc == 3 && string(argv[1]) == "--get-env" && !string_digits(argv[2]).empty()) {
    printf("%s", env_from_pid(stoul(string_digits(argv[2]), nullptr, 10)).c_str());
    printf("%s", "\r\n");
  }
  return 0;
}
