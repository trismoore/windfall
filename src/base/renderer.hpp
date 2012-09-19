#ifndef RENDERER_HPP
#define RENDERER_HPP

class Renderer {

  static Renderer * linkedListHead;
  Renderer * linkedListNext;

public:
  static void resizeAll(int w, int h);

  Renderer();
  ~Renderer();

  virtual void render(double deltaTime) =0;
  virtual void resize(int w, int h)=0;
};

#endif//RENDERER_HPP
