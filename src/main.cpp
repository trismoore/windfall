#include "console.hpp"
#include "config.hpp"
#include "ogl.hpp"

int main(int argc, char ** argv)
{
  console.log("Starting Windfall");

  console.log("Parsing configuration").indent();
  Config config;
  console.debug("Setting default config").indent();
  config.Set("windowwidth","1024").Set("windowheight","768").Set("windowfullscreen",0);
  console.outdent();
  console.debug("Getting command line overrides").indent();
  config.ParseArgs(argc,argv);
  console.outdent();
  console.outdent();

  console.log("Starting OpenGL").indent();
  ogl.Init();
  ogl.OpenWindow();
  console.outdent();

  console.log("Shutting down OpenGL").indent();
  ogl.CloseWindow();
  ogl.Terminate();
  console.outdent();

  return 0;

}