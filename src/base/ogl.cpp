#include "ogl.hpp"
#include "console.hpp"
#include "config.hpp"
#include "camera.hpp"
#include "renderer.hpp"

OGL ogl;
int  GLFWCALL glfwCallbackWindowClose( void );
void GLFWCALL glfwCallbackResize( int width, int height );
void GLFWCALL glfwCallbackKey( int key, int action );
void GLFWCALL glfwCallbackChar( int character, int action );
void GLFWCALL glfwCallbackMouseButton( int button, int action );
void GLFWCALL glfwCallbackMouseMove( int x, int y );
void GLFWCALL glfwCallbackMouseWheel( int pos );

bool OGL::running = true;

OGL::OGL() {}
OGL::~OGL() {}

void OGL::quit() { running = false; console.quitting(); }

void OGL::init()
{
  if (GL_FALSE == glfwInit()) throw "Failed to initialize GLFW";

  console.debug("Getting available video modes for fullscreen").indent();
  int nummodes;
  GLFWvidmode list[ 200 ];
  nummodes = glfwGetVideoModes( list, 200 );
  for (int i=0; i<nummodes; ++i) {
    float aspect = float(list[i].Width)/float(list[i].Height);
    const char *aspTxt = "?";
    if (aspect > 1.30 && aspect < 1.36) aspTxt="4:3";
    else if (aspect > 1.75 && aspect < 1.80) aspTxt="16:9";
    else if (aspect > 1.57 && aspect < 1.61) aspTxt="8:5";
    else if (aspect > 1.22 && aspect < 1.27) aspTxt="5:4";
    console.debugf("%4dx%4d (%4s, aspect=%.2f) %dbits (%dr,%dg,%db)", list[i].Width, list[i].Height, aspTxt, aspect,
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

  int major=3, minor=3, fsaa=config->getInt("window.fsaa",4);
  console.logf("Asking for %dxAA, OpenGL %d.%d",fsaa,major,minor);
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, fsaa);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, major );
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, minor );
  glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (GL_FALSE == glfwOpenWindow(config->getInt("window.width",1024),
				  config->getInt("window.height",768),
				  0,0,0,0,
				  32,0,
				  config->getInt("window.fullscreen",0) ? GLFW_FULLSCREEN : GLFW_WINDOW))
    throw "Failed to open window";

  glfwSetWindowTitle(config->getString("window.title","Windfall").c_str());
  glfwSwapInterval(config->getInt("window.vsync",1));

  glfwSetWindowCloseCallback(&glfwCallbackWindowClose);
  glfwSetWindowSizeCallback(&glfwCallbackResize);
  glfwSetKeyCallback(&glfwCallbackKey);
  glfwSetCharCallback(&glfwCallbackChar);
  glfwSetMouseButtonCallback(&glfwCallbackMouseButton);
  glfwSetMousePosCallback(&glfwCallbackMouseMove);
  glfwSetMouseWheelCallback(&glfwCallbackMouseWheel);
  logOpenGLErrors();

  // wrangle some extensions
  glewExperimental = GL_TRUE;
  if (GLEW_OK != glewInit()) throw "GLEW init failed";
  console.log("GLEW initialized OK");

  if (logOpenGLErrors()) console.warning("An error message 'GL Error 1280: invalid enumerant' is safe to ignore here (caused by GLEW)");
  int OpenGLVersion[2] = {0};
  glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
  glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
  console.logf("OpenGL Version %d.%d", OpenGLVersion[0],OpenGLVersion[1]);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  logOpenGLErrors();
}

void OGL::closeWindow()
{
  console.debug("Closing window");
  glfwCloseWindow();
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

void GLFWCALL glfwCallbackResize(int w, int h)
{
	console.debugf("OGL::Window resized to %dx%d", w,h);
	Camera::resize(w,h);
	Renderer::onResizeAll(w,h);
}

int GLFWCALL glfwCallbackWindowClose(void)
{
	console.error("Window closed!  Probably should quit nicely now...");
	OGL::quit();
	return GL_TRUE;
}

void GLFWCALL glfwCallbackKey( int key, int action )
{
	// TODO: remove this...
	if (key==GLFW_KEY_ESC) { console.error("ESC KEY pressed, quitting now"); OGL::quit(); }
	Renderer::onKeyAll(key, action);
}

void GLFWCALL glfwCallbackChar( int character, int action )
{
	Renderer::onCharAll(character, action);
}

void GLFWCALL glfwCallbackMouseButton( int button, int action )
{
	Renderer::onMouseButtonAll(button, action);
}

void GLFWCALL glfwCallbackMouseMove( int x, int y )
{
	Renderer::onMouseMoveAll(x,y);
}

void GLFWCALL glfwCallbackMouseWheel( int pos )
{
	Renderer::onMouseWheelAll(pos);
}
