#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <map>
#include <string>

#include "ogl.hpp"

class Texture {

  GLuint id;
  std::string filename;
  static std::map< GLuint, Texture* > bound;

public:

  void bind(GLuint unit);
  void loadDDS(const char* f);

  Texture();
  Texture(const char *f);
  ~Texture();

  void filterNearest();
  void filterLinear();
  void filterMipmap();

  void wrapRepeat() { wrap(GL_REPEAT); }
  void wrapMirror() { wrap(GL_MIRRORED_REPEAT); }
  void wrapClamp()  { wrap(GL_CLAMP); }
  void wrap(GLenum wrap);
};

#endif//TEXTURE_HPP