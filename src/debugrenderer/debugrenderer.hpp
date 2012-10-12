#ifndef DEBUGRENDERER_HPP
#define DEBUGRENDERER_HPP

#include "renderer.hpp"

class Config;
class VBO;
class Shader;

class DebugRenderer : public Renderer {

  bool enabled;

  VBO * vbo;
  Shader * shader;

public:

  DebugRenderer(Config* config);
  ~DebugRenderer();

  void render();
};

#endif//DEBUGRENDERER_HPP
