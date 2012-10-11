#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "awesome.hpp"

class Camera
{
	glm::vec3 pos, lookDirection, lookUp;
	glm::vec3 rotate; // pitch,roll,yaw
	float fov; // (horizontal)
	float nearP, farP;

	static float aspect;
	Awesome* awesome;

public:
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 VP; // = Projection*View

	Camera(Awesome* a);

	void setPos(glm::vec3 p);

	void yaw(float y);
	void pitch(float p);
	void roll(float r);
	void forward(float dist_per_second);
	void backward(float dist_per_second);
	void left(float dist_per_second);
	void right(float dist_per_second);

	void update();

	static void resize(const int w, const int h);

	AWESOME_FUNC(JSsetPos);
	AWESOME_FUNC(JSsetLook);
	AWESOME_FUNC(JSsetUp);
	AWESOME_FUNC(JSsetFOV);
};

#endif//CAMERA_H
