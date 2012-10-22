#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "awesome.hpp"

class Camera
{
	glm::vec3 rotate; // pitch,roll,yaw
	float fov; // (horizontal)
	float nearP, farP, nearFarDifferenceInv;

	static float aspect;
	Awesome* awesome;

	float frustum[6][4];
	void extractFrustumFromMatrix(const glm::mat4& mat);

public:
	glm::vec3 pos, lookDirection, lookUp;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 VP; // = Projection*View

	Camera(Awesome* a);

	// returns distance from front near clip plane, or <0 if outside frustum
	float sphereInFrustumDistance(const glm::vec3& centre, const float radius);
	// returns <0 if outside frustum, 0..1 if inside (0=near plane, 1=far plane)
	float getLOD(const glm::vec3& centre, const float radius);

	bool isSphereVisible(const glm::vec3 & centre, const float radius);

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
