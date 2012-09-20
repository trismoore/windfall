#include "console.hpp"
#include "config.hpp"
#include "ogl.hpp"
extern const char* g_versionString;

#include "testrenderer.hpp"
#include "awesome.hpp"
#include "useful.h"

double gTime, gdT;

int main(int argc, char ** argv)
{
  console.logf("Starting Windfall (%s)", g_versionString);

  Config config;
  console.log("Parsing configuration").indent();
    console.debug("Setting default config").indent();
      config.set("window.width","1024").set("window.height","768").set("window.fullscreen",0);
      config.set("window.title", std::string("Windfall (") + g_versionString + ")");
    console.outdent();
    console.debug("Getting command line overrides").indent();
      config.parseArgs(argc,argv);
    console.outdent();
  console.outdent();

  console.log("Starting OpenGL").indent();
    ogl.init();
    ogl.openWindow(&config);
  console.outdent();

  TestRenderer testrenderer;

  Awesome awesome(&config);
  awesome.registerCallbackFunction( L"UI", L"Quit", Awesomium::JSDelegate(&ogl,&OGL::Quit));
  awesome.loadFile(config.getString("ui.html", "html/ui.html"));
//  awesome.loadURL("http://www.google.com");

	console.setupCallback(&awesome);

//  glfwEnable(GLFW_STICKY_KEYS);
//  bool running=true;

  gTime = glfwGetTime();
  double timeLastFrame = gTime;
  double timeEndFrame = gTime;
  int frames = 0;
  double oneSecondTimer = 1.0;
  gdT = 0;

  while (OGL::isRunning()) {
    gTime = glfwGetTime();
    gdT = gTime - timeLastFrame;
    timeLastFrame = gTime;

    testrenderer.render();
    awesome.render();

    timeEndFrame = glfwGetTime();
    oneSecondTimer -= gdT;
    ++frames;
    if (oneSecondTimer < 0) {
      oneSecondTimer = 1.0;
//      console.debugf("%d frames rendered last second, [last frame dTime=%.3fms (%.1f fps), actual render time=%.3fms (%.1f fps)]",
//		     frames, 1000.f*gdT, 1.f/gdT, 1000.f*(timeEndFrame-gTime), 1.f/(timeEndFrame-gTime));
      frames=0;
    }

    glfwSwapBuffers();
  }

  console.log("Shutting down OpenGL").indent();
    ogl.closeWindow();
    ogl.terminate();
  console.outdent();

  return 0;

}
