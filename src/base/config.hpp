#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>

class Config {

  std::map<std::string, std::string> m_map;

public:
  Config& Set(std::string, std::string);
  Config& Set(std::string, int);
  Config& Set(std::string, float);

  std::string GetString(std::string);
  float GetFloat(std::string);
  int GetInt(std::string);

  std::ostream& Dump(std::ostream& out);

  void ParseArgs(int argc, char **argv);
};

std::ostream& operator<< (std::ostream& out, Config* c);

#endif//CONFIG_HPP
