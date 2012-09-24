#include "console.hpp"

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "awesome.hpp"
Console console;

Console::Console(char indentChar) : indentChar(indentChar)
{
  indentNumber=0;
  indentString[0]='\0';

  useBackBuffer = true;
  passLineToJS = false;

  logFile.open(LOG_FILENAME);
  logFile << "<html><head><title>Windfall log</title></head>\n"
           << "<link rel='stylesheet' href='data/html/css/windfall.css'>\n"
           << "<body class='log'>\n"
           << "<h1>Windfall log</h1>\n"
	   << "<ul class='log'>\n";

  logf("Started console for logging, writing to %s",LOG_FILENAME);
  indent();
}

Console::~Console()
{
  outdent();
/*  debug("Debug text test");
  log("Normal log text");
  warning("Warning text test");
  error("Error text test");
  fatal("FATAL text test");*/
  if (indentNumber>0) logf("Console is incorrectly indented! (ind=%d)",indentNumber);
  log("Closed console, end of log.");

  logFile << "</ul>";
  logFile << "Backbuffer: " << backBuffer.size() << " lines\n";
  logFile << "</body></html>\n";
  logFile.close();
}

Console& Console::indent()
{
  indentString[indentNumber]=indentChar;
  if (indentNumber<100) indentNumber++;
  indentString[indentNumber]='\0';
  logFile << "<li><ul>\n";
  if (useBackBuffer) backBuffer.push_back("<li><ul>");
  return *this;
}

Console& Console::outdent()
{
  if (indentNumber>0) indentNumber--;
  indentString[indentNumber]='\0';
  logFile << "</ul></li>\n";
  if (useBackBuffer) backBuffer.push_back("</li></ul>");
  return *this;
}

#define VA_TMPTEXT  char log_temp_text[1024]; \
                    va_list argptr; \
                    va_start(argptr, string); \
                    vsnprintf(log_temp_text, 1024, string, argptr); \
                    va_end(argptr);

Console& Console::debugf(const char *string, ...)   { VA_TMPTEXT; return debug(log_temp_text); }
Console& Console::logf(const char *string, ...)     { VA_TMPTEXT; return log(log_temp_text); }
Console& Console::warningf(const char *string, ...) { VA_TMPTEXT; return warning(log_temp_text); }
Console& Console::errorf(const char *string, ...)   { VA_TMPTEXT; return error(log_temp_text); }
Console& Console::fatalf(const char *string, ...)   { VA_TMPTEXT; return fatal(log_temp_text); }

Console& Console::debug(const std::string& s)   { return debug(s.c_str()); }
Console& Console::log(const std::string& s)     { return log(s.c_str()); }
Console& Console::warning(const std::string& s) { return warning(s.c_str()); }
Console& Console::error(const std::string& s)   { return error(s.c_str()); }
Console& Console::fatal(const std::string& s)   { return fatal(s.c_str()); }

Console& Console::debug(const char *s)   { return write(0,"debug",s); }
Console& Console::log(const char *s)     { return write(1,"log",s); }
Console& Console::warning(const char *s) { return write(2,"warning",s); }
Console& Console::error(const char *s)   { return write(3,"error",s); }
Console& Console::fatal(const char *s)   { return write(4,"fatal",s); }

Console& Console::write(const int log_level, const std::string& log_class, const std::string& string)
{
  assert(log_level >= 0);
  assert(log_level <= 4);
  static char consoleChars[] = "#>!EF";
  std::cout << consoleChars[log_level] << " " << indentString << string << "\n";
  std::string msg = "<li class='" + log_class + "'>" + string + "</li>";
  logFile << msg << "\n";
  if (useBackBuffer) backBuffer.push_back(msg);
  if (passLineToJS) {
	std::string l = "addToConsole(\"" + msg + "\");";
//	  std::string l = "$('#consoleWindow ul.log').append(\"" + msg + "\"); \
	    $(consoleWindow).animate({scrollTop: consoleWindow.scrollHeight}, 500);";
	  awesome->runJS(l);
  }
  return *this;
}

void Console::popBackBuffer(Awesomium::WebView* caller, const Awesomium::JSArguments& args)
{
	std::string JSONlines = "$('#consoleWindow ul.log').append(\"";
	for (std::vector<std::string>::iterator it = backBuffer.begin(); it != backBuffer.end(); ++it) {
/*		std::string l = "addLineToConsole(\""+*it+"\");";
		caller->executeJavascript(l);
		log(l);*/
		//std::string l="\"" + *it + "\",";
		std::string l = *it;
		JSONlines += l;
	}
	JSONlines += "\");";
	caller->executeJavascript(JSONlines);
	// now that we're loaded, we can now just add lines in the write method
	passLineToJS = true;
	// and this line should cause a scroll to the bottom
	logf("Console loaded, sent %d lines of back buffer", backBuffer.size());
}

void Console::addToLog(Awesomium::WebView* caller, const Awesomium::JSArguments& args)
{
	bool passLineToJSTmp = passLineToJS;
	passLineToJS = false;
	if (!args[0].isString()) warning("Console::addToLog called with non-string arg, expect useless output");
	std::string m(args[0].toString().begin(),args[0].toString().end());
	std::string t(args[1].toString().begin(),args[1].toString().end());
//printf("addToLog(%s,%s);\n",m.c_str(),t.c_str());
//	m="[C::aTL]" + m;

	char tchar = t.c_str()[0];
	switch (tchar) {
		case 'l': write(0,"debug",m); break;
		case 'i': write(1,"log",m); break;
		case 'w': write(2,"warning",m); break;
		case 'e': write(3,"error",m); break;
		default: write(0,"debug",std::string("Unknown message class: ")+t); write(0,"debug",m); break;
	}
	passLineToJS = passLineToJSTmp;
}

void Console::setupCallback(Awesome* a)
{
	awesome = a;
	awesome->registerCallbackFunction( L"UI", L"popBackBuffer", Awesomium::JSDelegate(this,&Console::popBackBuffer));
	awesome->registerCallbackFunction( L"UI", L"addToLog", Awesomium::JSDelegate(this, &Console::addToLog));
}

