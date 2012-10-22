#include <sstream>
#include <algorithm>

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
    console.logf("Config default (S) %s=%s", k.c_str(), def.c_str());
    return def;
  }
  else return getString(k);
}

float Config::getFloat(std::string k, float def)
{
  if (m_map.find(k) == m_map.end()) {
    console.logf("Config default (F) %s=%f", k.c_str(), def);
    return def;
  }
  else return getFloat(k);
}

int Config::getInt(std::string k, int def)
{
  if (m_map.find(k) == m_map.end()) {
    console.logf("Config default (I) %s=%i", k.c_str(), def);
    return def;
  }
  else return getInt(k);
}

void Config::parseArgs(int argc, char **argv, bool skipFirst)
{
  for (int i=skipFirst?1:0; i<argc; ++i) {
    // parse for config commands (probably of form "width=1920")
    std::string s(argv[i]);
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    size_t found = s.find("=");
    if (found != std::string::npos) {
      std::string k,v;
      k=s.substr(0,found);
      v=s.substr(found+1);
//printf("Config::Set %s,%s\n",k.c_str(),v.c_str());
      set(k,v);
    }
  }
}

void Config::loadFile(const char *filename)
{
//printf("Config::loadFIle %s\n",filename);
	FILE *fp = fopen(filename,"rt");
	if (!fp) return;
	char * line = 0;
	size_t len;
	ssize_t read;
	while ((read = getline(&line, &len, fp)) != -1) {
//printf("getline %d %s %d\n",read,line,len);
		if (line[0] != '#')
		parseArgs(1,&line,false);
	}
	free(line);
}
