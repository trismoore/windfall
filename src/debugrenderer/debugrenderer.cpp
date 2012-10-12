#include "debugrenderer.hpp"

#include "config.hpp"
#include "shader.hpp"
#include "vbo.hpp"
#include "console.hpp"

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
  shader->set("scale", 2.f / 4.0f/*number of outputs*/);

  console.outdent();
}

DebugRenderer::~DebugRenderer()
{

}

void DebugRenderer::render()
{
  if (!enabled) return;

  vbo->bind(shader);
  for (int i=0; i<4; ++i) {
    shader->set("offX", 2.0f*float(i)/4);
    shader->set("myTextureSampler", i);
    vbo->drawArrays(GL_TRIANGLE_STRIP);
  }
  logOpenGLErrors();
}
