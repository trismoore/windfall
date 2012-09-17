#ifndef OGL_HPP
#define OGL_HPP

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
using namespace glm;

class OGL {

public:

  OGL();
  ~OGL();

  void Init();
  void Terminate();

  void OpenWindow();
  void CloseWindow();
};

extern OGL ogl;

#endif//OGL_HPP
