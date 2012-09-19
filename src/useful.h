#ifndef USEFUL_H
#define USEFUL_H

extern const char* g_versionString;

//#define DATA_DIR "@PROJECT_BINARY_DIR@/data/"
#define DATA_DIR "data/"

#include <string>
#include <algorithm>

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}

#ifndef clamp
#define clamp(a,b,c) ( ((a)<(b)) ? (b) : ((a)>(c) ? (c) : (a)) )
#endif

static inline std::string substr(const std::string &str, int start, int length)
{
  if (start  < 0 ) start+=str.length();
  if (length < 0 ) length=str.length()+length-start;
  if (length < 0 ) return "";
  return str.substr(start,length);
}

static inline std::string substr(const std::string &str, int start)
{
  return substr(str,start,str.length());
}

#endif//USEFUL_H
