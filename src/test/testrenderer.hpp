#ifndef TESTRENDERER_HPP
#define TESTRENDERER_HPP

#include "renderer.hpp"

class TestRenderer : public Renderer {

public:

  TestRenderer();
  ~TestRenderer();

  void render(double deltaTime);
};

#endif//TESTRENDERER_HPP
