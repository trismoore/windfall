#ifndef SHADER_HPP
#define SHADER_HPP

#include <vector>
#include <map>

#include "ogl.hpp"

class Texture;
class Camera;

class Shader {

  void printErrors(GLuint ID);

  GLuint fragment;
  GLuint vertex;
  GLuint program;
  static GLuint activeProgram;

  std::vector< Texture* > textures;
  std::vector< GLuint > texUnits;
  GLuint unitCount;

  std::string filename;

  std::map< std::string, std::string > shaders; // shaders["vertex"] etc
  std::map< std::string, GLint > uniform_cache;
  std::map< std::string, GLint > attribute_cache;

  void load(const std::string& file);
  void load(const std::vector< std::string> & files);
  void loadSource(const std::string & source);
  void compile(GLuint shader, const std::string& source);
public:

  GLint getUniformLocation(const std::string& name);
  GLint getAttribLocation(const std::string& name);

  void set(const std::string& name, const float f) { glUniform1f(getUniformLocation(name), f); }

  // special case of setUniform: set the camera matrix
  void setCamera(Camera* camera); // set "VP" to camera ViewProjection matrix

  void use();

  Shader(const std::string& file);
  Shader(const std::vector< std::string> & files);
  ~Shader();
};

#endif//SHADER_HPP
