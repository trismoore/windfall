#ifndef OGL_HPP
#define OGL_HPP

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
using namespace glm;

class Config;

class OGL {

public:

  OGL();
  ~OGL();

  void init();
  void terminate();

  void resize(int w, int h);

  void openWindow(Config* config);
  void closeWindow();
};

extern OGL ogl;

#endif//OGL_HPP
