#include "testrenderer.hpp"

#include "ogl.hpp"
#include "vbo.hpp"

TestRenderer::TestRenderer()
{
GLfloat Vertices[] = {
        -0.8f, -0.8f, 0.0f, 1.0f,
         0.0f,  0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f
    };
 
    GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

	vbo = new VBO();
	vbo->pushData("in_Position", 4, 4, Vertices);
	vbo->pushData("in_Color", 4, 4, Colors);
	vbo->create();

	shader = new Shader("tri.shader");
}

TestRenderer::~TestRenderer()
{
	delete vbo;
}

extern double gTime;

void TestRenderer::render()
{
  glClearColor(0.2+0.2*sin(gTime),0.5,0.1,0.9);
  glClear(GL_COLOR_BUFFER_BIT);
vbo->bind(shader);
vbo->drawArrays(GL_TRIANGLES);
}
