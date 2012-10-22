#ifndef LANDSCAPE_HPP
#define LANDSCAPE_HPP

#include "renderer.hpp"
#include "ogl.hpp"

#include <vector>

class Config;
class Awesome;
class QTree;

class Landscape : public Renderer {

	QTree * qtreeRoot;

	bool cameraDragging;
	int cameraMouseX, cameraMouseY;

	Config* config;
	Awesome * awesome;

public:

	Landscape(Config*, Awesome*);
	~Landscape();

	float load();

	void render();
	void onMouseButton(int button, int action);
	void onMouseMove(int x, int y);
};

#endif//LANDSCAPE_HPP
