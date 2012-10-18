#ifndef LANDPATCH_HPP
#define LANDPATCH_HPP

#include "object.hpp"

class VBO;
class Shader;

class LandPatch : public Object
{
friend class Landscape;

	static VBO * vbo;
	static Shader * shader;

public:

	LandPatch(glm::vec3 p);
	LandPatch(const float x, const float y, const float z);
	~LandPatch();

	void draw();
};

#endif//LANDPATCH_HPP
