#ifndef OGL_HPP
#define OGL_HPP

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
using namespace glm;

#include "awesome.hpp"

#define logOpenGLErrors() _logOpenGLErrors(__FUNCTION__,__FILE__,__LINE__)
extern bool _logOpenGLErrors(const char *function, const char *file, const int line);

class Config;

class OGL {
private:
	static bool running;
public:

  OGL();
  ~OGL();

  void init();
  void terminate();

  void openWindow(Config* config);
  void closeWindow();

  static void quit();
  static bool isRunning() { return running; }
  void Quit(Awesomium::WebView* caller, const Awesomium::JSArguments& args);
};

extern OGL ogl;

#endif//OGL_HPP
