#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <string>

class Console
{
  char indentString[100];
  int indentNumber;
  char indentChar;

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

  Console& log(const std::string& string);
  Console& debug(const std::string& string);
  Console& warning(const std::string& string);
  Console& error(const std::string& string);
  Console& fatal(const std::string& string);

};

extern Console console;

#endif//CONSOLE_HPP