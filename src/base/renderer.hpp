#ifndef RENDERER_HPP
#define RENDERER_HPP

class Renderer {

public:

  Renderer();
  ~Renderer();

  virtual void render(double deltaTime) =0;
};

#endif//RENDERER_HPP
