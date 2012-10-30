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

LandPatch::LandPatch(const int x, const int z, const int sizeX, const int sizeZ)
	: sizeX(sizeX), sizeZ(sizeZ)
{
	if (sizeX == 1 && sizeZ == 1) {
		// calculate min/max height for this patch
		float mn=999,mx=-999;
		texHeights->getMinMaxRed(x*textureScaleX,z*textureScaleZ,(x+1)*textureScaleX,(z+1)*textureScaleZ,&mn,&mx);

		mn *= heightScale;
		mx *= heightScale;

		position = glm::vec3(x,heightScale * (mx+mn)/2.f,z); // position is XZ origin, but average height (left,close,middle)
		boundingBoxMin = glm::vec3(x,mn,z);
		boundingBoxMax = glm::vec3(x+1,mx,z+1);
		centre = 0.5f * (boundingBoxMin + boundingBoxMax);
	} else {
//printf("not calculating for %d,%d-%d,%d\n",x,z,sizeX,sizeZ);
		position = glm::vec3(x,0,z);
	}
}

LandPatch::~LandPatch()
{

}

void LandPatch::draw(int lod)
{
//printf("Patch at %f,%f +%.3f,%.3f\n", position.x,position.z,sizeX,sizeZ);
	shader->set2f("position", position.x, position.z);
	shader->set2f("size", sizeX,sizeZ);

//	vbo->drawArrays(GL_POINTS);

	int l = min(lod, numberOfLODLevels-1);

	vbo->drawElements(GL_TRIANGLES,
                          indicesLocations[l].stitchCount,
                          indicesLocations[l].stitchOffset);

        vbo->drawElements(GL_TRIANGLE_STRIP,
                          indicesLocations[l].stripCount,
                          indicesLocations[l].stripOffset);

}

