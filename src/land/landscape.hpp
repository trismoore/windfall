#ifndef LANDSCAPE_HPP
#define LANDSCAPE_HPP

#include "renderer.hpp"
#include "ogl.hpp"

#include <vector>

struct LandIndices {
	GLuint stitchOffset;
	GLuint stitchCount;
	GLuint stripOffset;
	GLuint stripCount;
};

class Shader;
class Texture;
class VBO;
class Config;

class Landscape : public Renderer {

  Shader * shader;
  Texture * texHeights;
  VBO * vbo;

  std::vector< LandIndices > indicesLocations;

public:

  Landscape(Config*);
  ~Landscape();

  void render();
};

#endif//LANDSCAPE_HPP
