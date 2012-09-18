#include "ogl.hpp"
#include "console.hpp"
#include "config.hpp"

OGL ogl;
void GLFWCALL GLFWWindowResizeCallback(int w, int h);

OGL::OGL()
{

}

OGL::~OGL()
{

}

void OGL::init()
{
  if (GL_FALSE == glfwInit()) throw "Failed to initialize GLFW";

  console.debug("Getting available video modes for fullscreen").indent();
  int nummodes;
  GLFWvidmode list[ 200 ];
  nummodes = glfwGetVideoModes( list, 200 );
  for (int i=0; i<nummodes; ++i) {
    console.debugf("%4dx%4d %d,%d,%d", list[i].Width, list[i].Height, list[i].RedBits, list[i].GreenBits, list[i].BlueBits);
  }
  console.outdent();
  GLFWvidmode desktop;
  glfwGetDesktopMode(&desktop);
  console.debugf("Desktop is %4dx%4d %d,%d,%d", desktop.Width, desktop.Height, desktop.RedBits, desktop.GreenBits, desktop.BlueBits);
}

void OGL::terminate()
{
  glfwTerminate();
}

void OGL::openWindow(Config* config)
{
  console.debug("Opening window");
  if (GL_FALSE == glfwOpenWindow(config->getInt("windowwidth",1024),
				  config->getInt("windowheight",768),
				  0,0,0,0,
				  32,0,
				  config->getInt("windowfullscreen",0) ? GLFW_FULLSCREEN : GLFW_WINDOW))
    throw "Failed to open window";

  glfwSetWindowTitle(config->getString("windowtitle","Windfall").c_str());
  glfwSwapInterval(config->getInt("waitforvsync",1));

  glfwSetWindowSizeCallback(GLFWWindowResizeCallback);
}

void OGL::closeWindow()
{
  console.debug("Closing window");
  glfwCloseWindow();
}

void OGL::resize(int w, int h)
{
  console.debugf("Window resized to %dx%d", w,h);
}

void GLFWCALL GLFWWindowResizeCallback(int w, int h)
{
  ogl.resize(w,h);
}