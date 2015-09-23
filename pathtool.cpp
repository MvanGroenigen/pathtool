#include <iostream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>

std::vector<std::string> strs;
std::vector<std::string> results;

bool dorealpath = true;

const char* PATH = "PATH" ;

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option(s)>"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message\n"
              << "\t--remove-nonexisting\t\tremove from path if directory does not exist\n"
              << "\t--no-real- path\t\tDont use realpath to clean up dirnames\n"
              << "\t-p  <path> // prepend path\n"
              << "\t<path> // prepend path\n"
              << "\t-pn <path> // prepend path (do not move if it exists)\n"
              << "\t-a  <path> // append path\n"
              << "\t-an <path> // append path (do not move if it exists)\n"
              << std::endl;
}

void add(std::string item , bool atEnd = true , bool replace = false) {
  char *buffer = NULL;
  std::vector<std::string>::iterator it;

  if( dorealpath && item.at(0) == '/' ) {
    buffer = canonicalize_file_name(item.c_str());
    if( buffer != NULL ) {
      item = std::string(buffer);
    }
    free(buffer);
  }

  it=std::find(results.begin(), results.end(), item);
  if(it != results.end()) {
    if(! replace ) return;
    results.erase(it);
  }

  if(atEnd) {
    results.push_back(item);
  } else {
    results.insert (results.begin(), item);
  }
  return;
}

int main(int argc, char* argv[])
{
  std::vector<std::string>::iterator it;

  if(const char* env_p = std::getenv(PATH))
  {
    boost::split(strs,env_p,boost::is_any_of(":"));
  }

  for (it = strs.begin(); it != strs.end(); ++it)
  {
    add(*it);
  }

  for (int i = 1; i < argc; ++i) {
    bool atEnd = false;
    bool replace = true;

    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help")) {
      show_usage(argv[0]);
      return 0;
    } else if (arg == "-p") {
      // no change is default
    } else if (arg == "-pn") {
      replace = false;
    } else if (arg == "-a") {
      atEnd = true;
    } else if (arg == "-an") {
      atEnd = true;
      replace = false;
    } else {
      i--; // move pointer one step back... this is not a parameter but a path
    } 
    if (i + 1 < argc) {
      i++;
      add(std::string(argv[i]),atEnd,replace);
    } else {
      std::cerr << "Error: " << arg << " requires a path behind it" << std::endl;
      show_usage(argv[0]);
      return 1;
    }  
  }

  std::string joined = boost::algorithm::join(results, ":");

  std::cout << joined << std::endl;
  return 0;
}

