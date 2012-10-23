#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "console.hpp"
#include "awesome.hpp"

float Camera::aspect = 4.f / 3.f;
extern double gdT;

#define TOP    0
#define BOTTOM 1 // we don't at present use TOP+BOTTOM, it's just 2D culling
#define LEFT   2
#define RIGHT  3
#define FAR    4
#define NEAR   5 // NEAR must be 5 for sphereInDistance and getLOD to work.
#define PLANE_TO_START_ON LEFT

Camera::Camera(Awesome* awesome) : awesome(awesome)
{
	pos = glm::vec3(0.1f,2.5f,0.1f);
	lookDirection = glm::normalize(glm::vec3(0.5f,-0.1,0.5f));
	lookUp = glm::normalize(glm::vec3(0.f,1.f,0.f));
	fov = 60;
	nearP =    0.01;
	farP =   100.0;
	nearFarDifferenceInv = 1.f/ (farP-nearP);

	awesome->createObject(L"camera");
	awesome->registerCallbackFunction( L"camera", L"setPos", Awesomium::JSDelegate(this, &Camera::JSsetPos));
	awesome->registerCallbackFunction( L"camera", L"setLook", Awesomium::JSDelegate(this, &Camera::JSsetLook));
	awesome->registerCallbackFunction( L"camera", L"setUp", Awesomium::JSDelegate(this, &Camera::JSsetUp));
	awesome->registerCallbackFunction( L"camera", L"setFOV", Awesomium::JSDelegate(this, &Camera::JSsetFOV));

	update();
}

void Camera::yaw(float y) { lookDirection = glm::rotate(lookDirection,y,lookUp); }
void Camera::pitch(float p) { 
	lookDirection = glm::rotate(lookDirection,p,glm::cross(lookDirection,lookUp));
	lookUp = glm::rotate(lookUp,p,glm::cross(lookDirection, lookUp));
}
void Camera::roll(float r) {
	lookUp = glm::rotate(lookUp,r,lookDirection);
}
void Camera::forward(float dist) { pos += lookDirection * dist; }
void Camera::backward(float dist) { pos -= lookDirection * dist; }
void Camera::left(float dist) { pos -= (glm::cross(lookDirection, lookUp) * dist); }
void Camera::right(float dist) { pos += (glm::cross(lookDirection, lookUp) * dist); }

void Camera::update()
{
	// slowly rotate back to upward = +Y
	float ang;
	glm::vec3 lookRight = glm::cross(lookDirection, lookUp);
	ang = lookRight[1];
	roll(ang * 30 * gdT);

	// from wikipedia, horizon (km) is 3.57 * sqrt(height*1000) away.
	farP = 3.57 * sqrtf((pos.y) * 1000);
	nearP = 0.0001 * farP;
//printf("h%.3f, farP%.3f nearP%.3f\n", pos.y, farP, nearP);

	ProjectionMatrix = glm::perspective(fov, aspect, nearP, farP);
	
	lookDirection = normalize(lookDirection);
	lookUp = normalize(lookUp);

	ViewMatrix = glm::lookAt(pos, lookDirection + pos, lookUp);

	VP = ProjectionMatrix * ViewMatrix;

	// extract the frustum from the VP
	extractFrustumFromMatrix(VP);

//printf("p%.2f,%.2f,%.2f lD%.2f,%.2f,%.2f lU%.2f,%.2f,%.2f a%.3f\n",pos.x,pos.y,pos.z, lookDirection.x,lookDirection.y,lookDirection.z, lookUp.x,lookUp.y,lookUp.z, ang);

	awesome->setObjectProperty(L"camera",L"posx",pos.x);
	awesome->setObjectProperty(L"camera",L"posy",pos.y);
	awesome->setObjectProperty(L"camera",L"posz",pos.z);
	awesome->setObjectProperty(L"camera",L"lookx",lookDirection.x);
	awesome->setObjectProperty(L"camera",L"looky",lookDirection.y);
	awesome->setObjectProperty(L"camera",L"lookz",lookDirection.z);
	awesome->setObjectProperty(L"camera",L"upx",lookUp.x);
	awesome->setObjectProperty(L"camera",L"upy",lookUp.y);
	awesome->setObjectProperty(L"camera",L"upz",lookUp.z);
	awesome->setObjectProperty(L"camera",L"fov",fov);
}

void normalizePlane(float * f) {
	float t = 1.f / sqrtf(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
	f[0]*=t;
	f[1]*=t;
	f[2]*=t;
	f[3]*=t;
}

void Camera::extractFrustumFromMatrix(const glm::mat4& mat)
{
	// Calculate the LEFT side
	frustum[LEFT][0] = mat[0][3] + mat[0][0];
	frustum[LEFT][1] = mat[1][3] + mat[1][0];
	frustum[LEFT][2] = mat[2][3] + + mat[2][0];
	frustum[LEFT][3] = mat[3][3] + + mat[3][0];

	// Calculate the RIGHT side
	frustum[RIGHT][0] = mat[0][3] - mat[0][0];
	frustum[RIGHT][1] = mat[1][3] - mat[1][0];
	frustum[RIGHT][2] = mat[2][3] - mat[2][0];
	frustum[RIGHT][3] = mat[3][3] - mat[3][0];

	// Calculate the TOP side
	frustum[TOP][0] = mat[0][3] - mat[0][1];
	frustum[TOP][1] = mat[1][3] - mat[1][1];
	frustum[TOP][2] = mat[2][3] - mat[2][1];
	frustum[TOP][3] = mat[3][3] - mat[3][1];

	// Calculate the BOTTOM side
	frustum[BOTTOM][0] = mat[0][3] + mat[0][1];
	frustum[BOTTOM][1] = mat[1][3] + mat[1][1];
	frustum[BOTTOM][2] = mat[2][3] + mat[2][1];
	frustum[BOTTOM][3] = mat[3][3] + mat[3][1];

	// Calculate the FRONT side
	frustum[NEAR][0] = mat[0][3] + mat[0][2];
	frustum[NEAR][1] = mat[1][3] + mat[1][2];
	frustum[NEAR][2] = mat[2][3] + mat[2][2];
	frustum[NEAR][3] = mat[3][3] + mat[3][2];

	// Calculate the BACK side
	frustum[FAR][0] = mat[0][3] - mat[0][2];
	frustum[FAR][1] = mat[1][3] - mat[1][2];
	frustum[FAR][2] = mat[2][3] - mat[2][2];
	frustum[FAR][3] = mat[3][3] - mat[3][2];

	for (int i=0; i<6; ++i)
		normalizePlane(frustum[i]);
}

float Camera::sphereInFrustumDistance(const glm::vec3& centre, const float radius)
{
	// this relies on the fact that NEAR is the last plane, i.e. NEAR=5.
	float d;
	for (int i=PLANE_TO_START_ON; i<6; ++i) {
		d = frustum[i][0]*centre.x + frustum[i][1]*centre.y + frustum[i][2]*centre.z + frustum[i][3];
		if (d<= -radius) return -1;
	}
	return d + radius; 
}

// same as sphereInFrusumDistance, but scaled to 0..1 for near..far
float Camera::getLOD(const glm::vec3& centre, const float radius)
{
	// this relies on the fact that NEAR is the last plane, i.e. NEAR=5.
	float d;
	for (int i=PLANE_TO_START_ON; i<6; ++i) {
		d = frustum[i][0]*centre.x + frustum[i][1]*centre.y + frustum[i][2]*centre.z + frustum[i][3];
		if (d<= -radius) return -1;
	}
	float r = clamp(((d + radius - nearP) * nearFarDifferenceInv), 0.f,1.f);
//printf("%.3f,%.3f,%.3f+%.3f d=%.3f, nfdi=%.3f ret=%.3f\n",centre.x,centre.y,centre.z,radius, d,nearFarDifferenceInv, r);
	return r;
}

bool Camera::isSphereVisible(const glm::vec3 & centre, const float radius) 
{
	for (int i=PLANE_TO_START_ON; i<6; ++i) {
		if (frustum[i][0]*centre.x + 
		    frustum[i][1]*centre.y + 
                    frustum[i][2]*centre.z + 
                    frustum[i][3]            <= -radius) return false;
	}
	return true;
}

void Camera::resize(const int w, const int h)
{
	if (h>0) aspect = float(w) / float(h);
	console.debugf("Camera resize %dx%d (aspect=%.3f)", w,h,aspect);
}

double getDouble(double * d, const Awesomium::JSValue & arg)
{
	if (!arg.isNumber()) return false;
	*d = arg.toDouble();
	return true; 
}

glm::vec3 getVec3(const char *func, const Awesomium::JSArguments& args)
{
	double x,y,z;
	if (args.size() >= 3
	 && getDouble(&x,args[0])
	 && getDouble(&y,args[1])
	 && getDouble(&z,args[2]))
		return glm::vec3(x,y,z);
	else
		console.errorf("%s needs three numbers (x,y,z)", func);
}

AWESOME_FUNC(Camera::JSsetPos)
{
	pos = getVec3("camera.setPos", args);
}

AWESOME_FUNC(Camera::JSsetLook)
{
	lookDirection = getVec3("camera.setLook", args);
}

AWESOME_FUNC(Camera::JSsetUp)
{
	lookUp = getVec3("camera.setUp", args);
}

AWESOME_FUNC(Camera::JSsetFOV)
{
	double f;
	if (args.size() == 1
         && getDouble(&f, args[0]))
		fov = f;
	else
		console.errorf("setFOV needs just one arg: the horizontal field of view");
}
