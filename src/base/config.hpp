#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>

class Config {

  std::map<std::string, std::string> m_map;

public:
  Config& set(std::string, std::string);
  Config& set(std::string, int);
  Config& set(std::string, float);

  std::string getString(std::string);
  float getFloat(std::string);
  int getInt(std::string);

  // get config values if they exist, or return the given default if not.
  std::string getString(std::string, std::string def);
  float getFloat(std::string, float def);
  int getInt(std::string, int def);

  void parseArgs(int argc, char **argv, bool skipFirst=true);
  void loadFile(const char *filename);
};

#endif//CONFIG_HPP
