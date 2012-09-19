#include "ogl.hpp"
#include "console.hpp"
#include "config.hpp"
#include "camera.hpp"
#include "renderer.hpp"

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
    console.debugf("%4dx%4d %dbits (%dr,%dg,%db)", list[i].Width, list[i].Height,
		   list[i].RedBits+list[i].GreenBits+list[i].BlueBits, list[i].RedBits, list[i].GreenBits, list[i].BlueBits);
  }
  console.outdent();
  GLFWvidmode desktop;
  glfwGetDesktopMode(&desktop);
  console.debugf("Desktop is %4dx%4d %dr,%dg,%db", desktop.Width, desktop.Height, desktop.RedBits, desktop.GreenBits, desktop.BlueBits);
}

void OGL::terminate()
{
  glfwTerminate();
}

void OGL::openWindow(Config* config)
{
  console.debug("Opening window");
  if (GL_FALSE == glfwOpenWindow(config->getInt("window.width",1024),
				  config->getInt("window.height",768),
				  0,0,0,0,
				  32,0,
				  config->getInt("window.fullscreen",0) ? GLFW_FULLSCREEN : GLFW_WINDOW))
    throw "Failed to open window";

  glfwSetWindowTitle(config->getString("window.title","Windfall").c_str());
  glfwSwapInterval(config->getInt("window.waitforvsync",1));

  glfwSetWindowSizeCallback(GLFWWindowResizeCallback);

  // wrangle some extensions
  if (GLEW_OK != glewInit()) throw "GLEW init failed";
  console.log("GLEW initialized OK");
}

void OGL::closeWindow()
{
  console.debug("Closing window");
  glfwCloseWindow();
}

void OGL::resize(int w, int h)
{
  console.debugf("OGL::Window resized to %dx%d", w,h);
  Camera::resize(w,h);
  Renderer::resizeAll(w,h);
}

void GLFWCALL GLFWWindowResizeCallback(int w, int h)
{
  ogl.resize(w,h);
}

bool _logOpenGLErrors(const char *function, const char *file, const int line)
{
  GLenum e = glGetError();
  bool r = false;
  while (e != GL_NO_ERROR) {
    r=true;
    console.errorf("[%s:%d function %s] GL Error %d: %s", file, line, function, e, gluErrorString(e));
    e = glGetError();
  }
  return r;
}
