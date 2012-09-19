#ifndef TESTRENDERER_HPP
#define TESTRENDERER_HPP

#include "renderer.hpp"

class TestRenderer : public Renderer {

public:

  TestRenderer();
  ~TestRenderer();

  void render(double deltaTime);
  void resize(int w, int h);
};

#endif//TESTRENDERER_HPP
