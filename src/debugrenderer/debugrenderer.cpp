#include "debugrenderer.hpp"

#include "config.hpp"
#include "shader.hpp"
#include "vbo.hpp"
#include "console.hpp"

// which texture units to display
#define START_UNIT      1
#define NUMBER_OF_UNITS 4

DebugRenderer::DebugRenderer(Config* config)
{
  enabled = config->getInt("debug",0);
  if (!enabled) return;

  console.logf("DebugRenderer loading").indent();

  GLfloat GTextures[] = {   0, 0,   0, 1,   1, 0,   1, 1 };

  vbo = new VBO();
  vbo->pushData("vertexUV", 2, 4, GTextures);
  vbo->create();

  shader = new Shader("debug.shader");
  shader->setf("scale", 2.f / float(NUMBER_OF_UNITS));

  console.outdent();
}

DebugRenderer::~DebugRenderer()
{

}

void DebugRenderer::render()
{
  if (!enabled) return;

  vbo->bind(shader);
  for (int i=START_UNIT; i<START_UNIT+NUMBER_OF_UNITS; ++i) {
    shader->setf("offX", 2.0f*float(i-START_UNIT)/float(NUMBER_OF_UNITS));
    shader->seti("myTextureSampler", i);
    vbo->drawArrays(GL_TRIANGLE_STRIP);
  }
  logOpenGLErrors();
}
