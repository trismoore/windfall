#ifndef LANDSCAPE_HPP
#define LANDSCAPE_HPP

#include "renderer.hpp"
#include "ogl.hpp"

#include <vector>

struct LandIndices {
	GLuint stitchOffset;
	GLuint stitchCount;
	GLuint stripOffset;
	GLuint stripCount;
};

class Shader;
class Texture;
class VBO;
class Config;

class Landscape : public Renderer {

	Shader * shader;
	Texture * texHeights;
	VBO * vbo;

	std::vector< LandIndices > indicesLocations;

	bool cameraDragging;
	int cameraMouseX, cameraMouseY;
	int numberOfLODLevels;

public:

	Landscape(Config*);
	~Landscape();

	void render();
	void onMouseButton(int button, int action);
	void onMouseMove(int x, int y);
};

#endif//LANDSCAPE_HPP
