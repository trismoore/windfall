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

public:

	LandPatch(const float x, const float z);
	~LandPatch();

	void draw();
};

#endif//LANDPATCH_HPP
