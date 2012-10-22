#include "landpatch.hpp"
#include "glm/glm.hpp"
#include "shader.hpp"
#include "vbo.hpp"
#include "camera.hpp"
#include "texture.hpp"

VBO * LandPatch::vbo=0;
Shader * LandPatch::shader=0;
Texture * LandPatch::texHeights=0;
Texture * LandPatch::texGround=0;
Texture * LandPatch::texGroundDetail=0;
std::vector< LandIndices > LandPatch::indicesLocations;
int LandPatch::numberOfLODLevels=0;
float LandPatch::textureScaleX, LandPatch::textureScaleZ;
float LandPatch::heightScale;

extern Camera* g_camera;

LandPatch::LandPatch(const float x, const float z)
{
	// calculate min/max height for this patch
	float mn=999,mx=-999;
	texHeights->getMinMaxRed(x*textureScaleX,z*textureScaleZ,(x+1)*textureScaleX,(z+1)*textureScaleZ,&mn,&mx);

	position = glm::vec3(x,heightScale * (mx+mn)/2.f,z); // position is XZ origin, but average height (left,close,middle)
	boundingBoxMin = glm::vec3(x,mn,z);
	boundingBoxMax = glm::vec3(x+1,mx,z+1);
	centre = 0.5f * (boundingBoxMin + boundingBoxMax);
}

LandPatch::~LandPatch()
{

}

void LandPatch::draw()
{
//printf("Patch at %f,%f\n", position.x,position.z);
	shader->set2f("position", position.x, position.z);

//	vbo->drawArrays(GL_POINTS);

	float distance = glm::distance(centre, g_camera->pos);
	int l;
	if (distance < 0.8) l=0;       // slightly favour close distances.  we're within this patch's sphere at 0.707 or there abouts
	else if (distance < 1.2) l=1;
	else l = min(numberOfLODLevels-1, int(distance*distance*1.4));
//printf("%.3f,%.3f d%.3f l%d\n", position.x,position.z, distance, l);


	vbo->drawElements(GL_TRIANGLES,
                          indicesLocations[l].stitchCount,
                          indicesLocations[l].stitchOffset);

        vbo->drawElements(GL_TRIANGLE_STRIP,
                          indicesLocations[l].stripCount,
                          indicesLocations[l].stripOffset);

}

