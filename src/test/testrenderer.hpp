#ifndef TESTRENDERER_HPP
#define TESTRENDERER_HPP

#include "renderer.hpp"
#include "vbo.hpp"
#include "shader.hpp"

class TestRenderer : public Renderer {
	VBO *vbo;
	Shader *shader;
public:

  TestRenderer();
  ~TestRenderer();

  void render();
};

#endif//TESTRENDERER_HPP
