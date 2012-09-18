#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <string>
#include <vector>
#include <fstream>

// Console is responsible for all logging
// (it's not called log because that's a maths thing)
// it writes to stdout, html to a log file and holds a list of strings
// that the in-game console grabs once it's loaded

#define LOG_FILENAME  "windfall_log.html"

class Console
{
  char indentString[100];
  int indentNumber;
  char indentChar;

  bool useBackBuffer;
  std::vector <std::string> backBuffer;

  std::ofstream logFile;

public:

  Console(char indentCharacter=' ');
  ~Console();

  Console& indent();
  Console& outdent();

  Console& log(const char *string);
  Console& debug(const char *string);
  Console& warning(const char *string);
  Console& error(const char *string);
  Console& fatal(const char *string);

  Console& logf(const char *string, ...);
  Console& debugf(const char *string, ...);
  Console& warningf(const char *string, ...);
  Console& errorf(const char *string, ...);
  Console& fatalf(const char *string, ...);

  Console& log(const std::string& string);
  Console& debug(const std::string& string);
  Console& warning(const std::string& string);
  Console& error(const std::string& string);
  Console& fatal(const std::string& string);

  Console& write(const int log_level, const std::string& log_class, const std::string& string);
};

extern Console console;

#endif//CONSOLE_HPP