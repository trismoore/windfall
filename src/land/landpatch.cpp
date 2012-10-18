#include "landpatch.hpp"
#include "glm/glm.hpp"
#include "shader.hpp"
#include "vbo.hpp"

VBO * LandPatch::vbo;
Shader * LandPatch::shader;

LandPatch::LandPatch(glm::vec3 p)
{
	position = p;
}

LandPatch::LandPatch(const float x, const float y, const float z)
{
	position = glm::vec3(x,y,z);
}

LandPatch::~LandPatch()
{

}

void LandPatch::draw()
{
//printf("Patch at %f,%f\n", position.x,position.z);
	shader->set2f("position", position.x, position.z);

	vbo->drawArrays(GL_POINTS);
}

