#ifndef SHADER_HPP
#define SHADER_HPP

#include <vector>
#include <map>

#include "ogl.hpp"

class Texture;
class Camera;

class Shader {

  std::string name; // so that we can access them from JS
  static Shader *root; // a linked list
  Shader *next;

  void printErrors(GLuint ID);

  GLuint fragment;
  GLuint vertex;
  GLuint program;
  static GLuint activeProgram;

  std::vector< std::string > filenames;       // filenames[0] = file we asked for, others are includes
  std::map< std::string, std::string > shaders; // shaders["vertex"] etc
  std::map< std::string, GLint > uniform_cache;
  std::map< std::string, GLint > attribute_cache;

  void getFile(std::string& source, const std::string& file); // grab and append a file to source
  void compile(GLuint shader, const std::string& source, const std::string& addToTop);
public:
  void load(const std::string& file);    // take a file, read it and then loadSource it
  void loadSource(const std::string& shadername, const std::string & source); // take a string, parse into vert/frag and compile. must give a name to the shader

  GLint getUniformLocation(const std::string& name);
  GLint getAttribLocation(const std::string& name);

  void setf(const std::string& name, const float f);
  void seti(const std::string& name, const int i);

  // special case of setUniform: set the camera matrix
  void setCamera(Camera* camera); // set "VP" to camera ViewProjection matrix

  void use();

  Shader(const std::string& file);
  ~Shader();

  static Shader* findShader(const std::string& shaderName);
  static void seti(const std::string& shaderName, const std::string& attribute, const int i);
  static void setf(const std::string& shaderName, const std::string& attribute, const float f);
  static void listShaders();
};

#endif//SHADER_HPP
