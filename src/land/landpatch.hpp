#ifndef LANDPATCH_HPP
#define LANDPATCH_HPP

#include "object.hpp"
#include "ogl.hpp"

struct LandIndices {
	GLuint stitchOffset;
	GLuint stitchCount;
	GLuint stripOffset;
	GLuint stripCount;
};

class VBO;
class Shader;
class Texture;

class LandPatch : public Object
{
friend class Landscape;
friend class QTree;

	static Shader * shader;
	static Texture * texHeights;
	static Texture * texGround;
	static Texture * texGroundDetail;
	static VBO * vbo;

	static std::vector< LandIndices > indicesLocations;
	static int numberOfLODLevels;
	static float textureScaleX,textureScaleZ,heightScale;

	glm::vec3 centre;
	float sizeX, sizeZ;

public:

	LandPatch(const int x, const int z, const int sizeX, const int sizeZ);
	~LandPatch();

	void draw(int lod);
};

#endif//LANDPATCH_HPP
