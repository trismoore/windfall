#include "testrenderer.hpp"

#include "ogl.hpp"

TestRenderer::TestRenderer()
{

}

TestRenderer::~TestRenderer()
{

}

void TestRenderer::render()
{
  glClearColor(0.2,0.5,0.1,0.9);
  glClear(GL_COLOR_BUFFER_BIT);

}
