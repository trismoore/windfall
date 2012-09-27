#include <iostream>
#include <fstream>
#include <sstream>
#include "shader.hpp"
#include "camera.hpp"
#include "console.hpp"
#include "useful.h"

GLuint Shader::activeProgram = -1;

Shader::Shader(const std::string& file)
  : fragment(0),vertex(0),program(0), unitCount(0)
{
  load(file);
}

Shader::~Shader()
{
  glDeleteShader(vertex);
  glDeleteShader(fragment);
  glDeleteProgram(program);
}

void Shader::load(const std::string &file)
{
  uniform_cache.clear();
  filename = std::string(DATA_DIR) + "/shaders/" + file;
  console.logf("Shader::Load <a href='%s'>%s</a>", filename.c_str(),file.c_str());
  console.debugf("Shader::Load loading from filename %s", filename.c_str());
  std::ifstream f(filename.c_str());
  if (f.fail()) {
    throw "Can't open shader file!";
  }
  console.indent();
  std::string tmp;
  std::string line;
  std::string source;
  std::string type = "unknown";
  int i = 0;
  // read it in and pre-process it
  while (std::getline(f,tmp)) {
    ++i;
    line = tmp;
    trim(line);
    if (line.size() > 0) {
      if (substr(line,-1).compare(":")==0) {
        type = substr(line,0,-1);
        console.debugf("Shader <em>%s</em> starting section <em>%s</em>", file.c_str(), type.c_str());
      } else {
        // i hate c++ strings
        std::ostringstream oss;
        oss << "#line " << i << '\n' << line << '\n';
        shaders[type] += oss.str();
	console.debugf("<code>%s %03d: %s</code>",type.c_str(), i, tmp.c_str());
      }
    }
  }
  // create shaders
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  vertex = glCreateShader(GL_VERTEX_SHADER);
  compile(vertex, shaders["vertex"]);
  compile(fragment, shaders["fragment"]);
  // attach them
  program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  // and link it all up
  glLinkProgram(program);
  GLint result = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &result);

  console.outdent();
  if (!result) printErrors(program);
  logOpenGLErrors();
}

void Shader::compile(GLuint shader, const std::string& source)
{
  // add some standard bits
  std::string added = "#version 330\n";
  added += source;
  const char *p = added.c_str();
  //LOG_PRINTF("Compiling source:\n%s\n",p);
  glShaderSource(shader, 1, &p, NULL);
  glCompileShader(shader);
  GLint result = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (!result) printErrors(shader);
}

GLint Shader::getUniformLocation(const std::string& name)
{
  std::map< std::string, GLint >::iterator it;
  it = uniform_cache.find(name);
  if (it != uniform_cache.end()) {
    //LOG_PRINTF("Uniform location for %s: cached %d", name.c_str(), it->second);
    return it->second;
  } else {
    use();
    GLuint l = glGetUniformLocation(program, name.c_str());
    uniform_cache[name] = l;
    console.logf("Program %d UniformLocation for %s: not cached %d", program, name.c_str(), l);
    logOpenGLErrors();
    return l;
  }
}

GLint Shader::getAttribLocation(const std::string& name)
{
  std::map< std::string, GLint >::iterator it;
  it = attribute_cache.find(name);
  if (it != attribute_cache.end()) {
    //LOG_PRINTF("Attribute location for %s: cached %d", name.c_str(), it->second);
    return it->second;
  } else {
    use();
    GLuint l = glGetAttribLocation(program, name.c_str());
    attribute_cache[name] = l;
    console.logf("Program %d AttribLocation for %s: not cached %d", program, name.c_str(), l);
    logOpenGLErrors();
    return l;
  }
}

void Shader::use()
{
  if (activeProgram != program) {
    glUseProgram(program);
    activeProgram = program;
  }
  if (logOpenGLErrors())
    console.errorf("Shader::Use errors in program %d", program);
  // set up textures here too
}

void Shader::printErrors(GLuint ID)
{
  // GL returns a massive string of errors.
  // chop them into \n terminated strings and LOG
  char tmp[1024];
  int length = 0;
  glGetShaderInfoLog(ID, 1024, &length, tmp);
  std::stringstream ss(tmp);
  std::string i;
  while (std::getline(ss, i)) {
    console.errorf("ShaderError in %d: %s",ID,i.c_str());
  }
}

void Shader::setCamera(Camera* camera)
{
  use();
  logOpenGLErrors();
  GLuint MatrixID = getUniformLocation("VP");
  logOpenGLErrors();
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &camera->VP[0][0]);
  logOpenGLErrors();
}
