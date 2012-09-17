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

Config& Config::Set(std::string n, std::string s)
{
  console.debug("Config::Set " + n + " = " + s);
  m_map[n]=s;
  return *this;
}

Config& Config::Set(std::string n, int i)
{
  std::stringstream ss;
  std::string str;
  ss << i;
  ss >> str;
//  m_map[n] = str;
  Set(n,str);
  return *this;
}

Config& Config::Set(std::string n, float f)
{
  std::stringstream ss;
  std::string str;
  ss << f;
  ss >> str;
//  m_map[n] = str;
  Set(n,str);
  return *this;
}

std::string Config::GetString(std::string n)
{
  return m_map[n];
}

float Config::GetFloat(std::string n)
{
  return ss_ato<float>(m_map[n]);
}

int Config::GetInt(std::string n)
{
  return ss_ato<int>(m_map[n]);
}

std::ostream& operator<< (std::ostream& out, Config* c)
{
  return c->Dump(out);
}

std::ostream& Config::Dump(std::ostream& out)
{
  std::map<std::string, std::string>::iterator it;
  for (it=m_map.begin(); it!=m_map.end(); ++it) {
    out << (*it).first << " = " << (*it).second << std::endl;
  }
  return out;
}

void Config::ParseArgs(int argc, char **argv)
{
  for (int i=1; i<argc; ++i) {
    // parse for config commands (probably of form "width=1920")
    std::string s(argv[i]);
    size_t found = s.find("=");
    if (found != std::string::npos) {
      std::string k,v;
      k=s.substr(0,found);
      v=s.substr(found+1);
      Set(k,v);
    }
  }
}
