#ifndef VBO_HPP
#define VBO_HPP

#include <string>
#include <vector>
#include "ogl.hpp"

struct Pointer {
  std::string name;
  int size;
  int offset;
};
class Shader;

class VBO {
  std::vector< Pointer > pointers;

  GLuint buffer;
  GLuint elementbuffer;
  GLuint VAO;

  int count;
  std::vector< GLfloat > data;
  std::vector< GLuint > indices;

  static bool * VertexAttribArrayCurrent;
  static bool * VertexAttribArrayWanted;
  static int numVertexAttribs; //= glGet(GL_MAX_VERTEX_ATTRIBS)
  static VBO * quad;
public:
  static void setupQuad();
  static void drawQuad(Shader * shader);
  void bind(Shader* shader);
  void pushData(const std::string &n, const int numElementsPerVertex, const int numVertices, const GLfloat* d);
  void pushIndices(const int count, const GLuint* i);
  void create();
  void drawArrays(GLenum type);
  void drawElements(GLenum type, int count, int offset);
  void drawElements(GLenum type); // all of 'em

  VBO();
  ~VBO();
};

#endif//VBO_HPP
