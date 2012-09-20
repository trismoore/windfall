#include "vbo.hpp"
#include "shader.hpp"
#include "console.hpp"

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

// FIXME: make sure all indices are < 65536 and convert to using GL_UNSIGNED_SHORT instead.

bool * VBO::VertexAttribArrayCurrent = NULL;
bool * VBO::VertexAttribArrayWanted = NULL;
int VBO::numVertexAttribs = 0;

VBO::VBO()
  : buffer(0), count(0), elementbuffer(0)
{
  if (!VertexAttribArrayCurrent) {
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numVertexAttribs);
    VertexAttribArrayCurrent = new bool[numVertexAttribs];
    VertexAttribArrayWanted = new bool[numVertexAttribs];
    for (int i=0; i<numVertexAttribs; ++i) {
      VertexAttribArrayCurrent[i]=false;
      VertexAttribArrayWanted[i]=false;
    }
    console.logf("No AttribArray yet, created one %d big", numVertexAttribs);
    logOpenGLErrors();
  }
}

VBO::~VBO()
{

}

void VBO::bind(Shader *shader)
{
  assert(buffer);
  assert(shader);

  shader->use();
  logOpenGLErrors();
  for (int i=0; i<numVertexAttribs; ++i) VertexAttribArrayWanted[i]=false;
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  logOpenGLErrors();
  glBindVertexArray(VAO);
  logOpenGLErrors();
  std::vector< Pointer >::iterator it;
  for (it = pointers.begin(); it != pointers.end(); ++it) {
    Pointer &p = *it;
    GLuint loc = shader->getAttribLocation(p.name);
    logOpenGLErrors();
    VertexAttribArrayCurrent[loc] = true;
    VertexAttribArrayWanted[loc] = true;
    glVertexAttribPointer(loc, p.size, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(p.offset * sizeof(float)));
    if (logOpenGLErrors())
      console.errorf("buffer=%d; loc=%d; p.size=%d; offset=%d floats (%d bytes)", buffer, loc, p.size, p.offset, p.offset*sizeof(float));
    glEnableVertexAttribArray(loc);
    if (logOpenGLErrors())
      console.errorf("buffer=%d; loc=%d; p.size=%d; offset=%d floats (%d bytes)", buffer, loc, p.size, p.offset, p.offset*sizeof(float));
  }
  // disable the ones we don't want.
  for (int i=0; i<numVertexAttribs; ++i) {
    if (VertexAttribArrayCurrent[i] && !VertexAttribArrayWanted[i]) {
      glDisableVertexAttribArray(i);
      VertexAttribArrayCurrent[i] = false;
    }
  }
  logOpenGLErrors();
}

void VBO::drawArrays(GLenum type)
{
  glDrawArrays(type, 0, count);
}

void VBO::drawElements(GLenum type)
{
  drawElements(type, indices.size(), 0);
}

void VBO::drawElements(GLenum type, int count, int offset)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glDrawElements(type, count, GL_UNSIGNED_INT, (void*)offset);
}

void VBO::pushData(const std::string &n, const int numElementsPerVertex, const int numVertices, const GLfloat* d)
{
  Pointer p;
  p.name = n;
  p.size = numElementsPerVertex;
  p.offset = data.size();
  pointers.push_back(p);

  if (count > 0 && numVertices!=count) {
    console.errorf("VBO PushData: %s, %d, %d != %d - vertex count mismatch in data!", n.c_str(),numElementsPerVertex,numVertices, count);
    console.log("This could be messy - the vertex counts don't match for the different pointers, so GL will use overlapping data.");
  }
  count = numVertices;

  data.insert(data.end(), d, d+numVertices*numElementsPerVertex);
  console.debugf("VBO::PushData %s %d*%d.  Data now %d floats", n.c_str(), numElementsPerVertex, numVertices, data.size());

//  for (int i=0; i<data.size(); ++i)
//    LOG_PRINTF("data[%3d]: %.3f ", i,data[i]);
}

void VBO::pushIndices(const int count, const GLuint* i)
{
  indices.insert(indices.end(), i, i+count);
  console.debugf("VBO::PushIndices: %d pushed, total now %d", count, indices.size());
}

void VBO::create()
{
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1,&buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
  console.debugf("Creating GL_ARRAY_BUFFER from %d pointers and data length %lu floats -> VAO %d, buffer %d", pointers.size(), data.size(), VAO, buffer);
  logOpenGLErrors();

  if (indices.size() > 0) {
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    console.debugf("Creating GL_ELEMENT_ARRAY_BUFFER for %d indices -> %d", indices.size(), elementbuffer);
    logOpenGLErrors();
  }
}
