#include "console.hpp"

#include <iostream>
#include <stdio.h>

Console console;

Console::Console(char indentChar) : indentChar(indentChar)
{
  indentNumber=0;
  indentString[0]='\0';
  log("Started console for logging");
  indent();
}

Console::~Console()
{
  outdent();
  if (indentNumber>0) log("Console is incorrectly indented!");
  log("Closed console, end of log.");
}

Console& Console::indent()
{
  indentString[indentNumber]=indentChar;
  if (indentNumber<100) indentNumber++;
  indentString[indentNumber]='\0';
  return *this;
}

Console& Console::outdent()
{
  if (indentNumber>0) indentNumber--;
  indentString[indentNumber]='\0';
  return *this;
}

Console& Console::log(const char *s)
{
  printf("#%s%s\n",indentString,s);
  return *this;
}

Console& Console::log(const std::string& s)
{
  std::cout << "#" << indentString << s << std::endl;
  return *this;
}

// **FIXME**
Console& Console::debug(const char *string)   { return log(string); }
Console& Console::warning(const char *string) { return log(string); }
Console& Console::error(const char *string)   { return log(string); }
Console& Console::fatal(const char *string)   { return log(string); }

Console& Console::debug(const std::string& string)   { return log(string); }
Console& Console::warning(const std::string& string) { return log(string); }
Console& Console::error(const std::string& string)   { return log(string); }
Console& Console::fatal(const std::string& string)   { return log(string); }

