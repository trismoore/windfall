#include <sstream>

#include "config.hpp"
#include "console.hpp"

// convert string to (int,float,etc)
// use:  int i=ss_ato<int>(str)
template<typename RT, typename T, typename Trait, typename Alloc>
RT ss_ato( const std::basic_string<T, Trait, Alloc>& the_string )
{
    std::basic_istringstream< T, Trait, Alloc> temp_ss(the_string);
    RT num;
    temp_ss >> num;
    return num;
}

Config& Config::set(std::string n, std::string s)
{
  console.debug("Config::set " + n + " = " + s);
  m_map[n]=s;
  return *this;
}

Config& Config::set(std::string n, int i)
{
  std::stringstream ss;
  std::string str;
  ss << i;
  ss >> str;
//  m_map[n] = str;
  set(n,str);
  return *this;
}

Config& Config::set(std::string n, float f)
{
  std::stringstream ss;
  std::string str;
  ss << f;
  ss >> str;
//  m_map[n] = str;
  set(n,str);
  return *this;
}

std::string Config::getString(std::string n)
{
  return m_map[n];
}

float Config::getFloat(std::string n)
{
  return ss_ato<float>(m_map[n]);
}

int Config::getInt(std::string n)
{
  return ss_ato<int>(m_map[n]);
}

std::string Config::getString(std::string k, std::string def)
{
  if (m_map.find(k) == m_map.end()) {
    console.debugf("Config::getString returning default (%s) for key '%s'", def.c_str(), k.c_str());
    return def;
  }
  else return getString(k);
}

float Config::getFloat(std::string k, float def)
{
  if (m_map.find(k) == m_map.end()) {
    console.debugf("Config::getFloat returning default (%f) for key '%s'", def, k.c_str());
    return def;
  }
  else return getFloat(k);
}

int Config::getInt(std::string k, int def)
{
  if (m_map.find(k) == m_map.end()) {
    console.debugf("Config::getInt returning default (%d) for key '%s'", def, k.c_str());
    return def;
  }
  else return getInt(k);
}

void Config::parseArgs(int argc, char **argv)
{
  for (int i=1; i<argc; ++i) {
    // parse for config commands (probably of form "width=1920")
    std::string s(argv[i]);
    size_t found = s.find("=");
    if (found != std::string::npos) {
      std::string k,v;
      k=s.substr(0,found);
      v=s.substr(found+1);
      set(k,v);
    }
  }
}
