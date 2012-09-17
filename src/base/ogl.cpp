#include "ogl.hpp"
#include "console.hpp"

OGL ogl;

OGL::OGL()
{

}

OGL::~OGL()
{

}

void OGL::Init()
{
  console.log("OGL::Init()");
  if (GL_FALSE == glfwInit()) throw "Failed to initialize GLFW";
}

void OGL::Terminate()
{
  glfwTerminate();
}

void OGL::OpenWindow()
{
  console.debug("Opening window");
  if (GL_FALSE == glfwOpenWindow(0,0,0,0,0,0,0,0,GLFW_WINDOW)) throw "Failed to open window";
}

void OGL::CloseWindow()
{
  console.debug("Closing window");
  glfwCloseWindow();
}