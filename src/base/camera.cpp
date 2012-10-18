#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "console.hpp"
#include "awesome.hpp"

float Camera::aspect = 4.f / 3.f;
extern double gdT;

Camera::Camera(Awesome* awesome) : awesome(awesome)
{
	pos = glm::vec3(-0.1f,0.3f,-0.1);
	lookDirection = glm::normalize(glm::vec3(0.5f,-0.1,0.5f));
	lookUp = glm::normalize(glm::vec3(0.f,1.f,0.f));
	
	fov = 60;
	nearP =    0.01;
	farP =   200.0;

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
	ProjectionMatrix = glm::perspective(fov, aspect, nearP, farP);

	// slowly rotate back to upward = +Y
	float ang;
	//ang = glm::angle(lookUp, glm::vec3(0,1,0));
	glm::vec3 lookRight = glm::cross(lookDirection, lookUp);
	ang = lookRight[1];
	roll(ang * 30 * gdT);

//	glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
	ViewMatrix = glm::mat4(); // identity

	/*// rotate Y then X then Z
	lookDirection = glm::vec3(0.f,0.f,-1.f);
	lookDirection = glm::rotateY(lookDirection, rotate.y);
	lookDirection = glm::rotateX(lookDirection, rotate.x);
	lookDirection = glm::rotateZ(lookDirection, rotate.z);
		
	lookUp = glm::vec3(0.f,1.f,0.f);
	lookUp = glm::rotateY(lookUp, rotate.y);
	lookUp = glm::rotateX(lookUp, rotate.x);
	lookUp = glm::rotateZ(lookUp, rotate.z);
*/
	ViewMatrix = glm::lookAt(pos, lookDirection + pos, lookUp);
/*
	ViewMatrix = glm::rotate(ViewMatrix, rotate.y, glm::vec3(0.f,1.f,0.f));
	ViewMatrix = glm::rotate(ViewMatrix, rotate.z, glm::vec3(0.f,0.f,1.f));
	ViewMatrix = glm::rotate(ViewMatrix, rotate.x, glm::vec3(1.f,0.f,0.f));

	ViewMatrix = glm::translate(ViewMatrix, pos);
	
	lookRight     = glm::vec3(ViewMatrix * glm::vec4(1,0,0,0));
	lookDirection = glm::vec3(ViewMatrix * glm::vec4(0,0,-1,0));
*/
	VP = ProjectionMatrix * ViewMatrix;

//printf("p%.2f,%.2f,%.2f lD%.2f,%.2f,%.2f lU%.2f,%.2f,%.2f a%.3f\n",pos.x,pos.y,pos.z, lookDirection.x,lookDirection.y,lookDirection.z, lookUp.x,lookUp.y,lookUp.z, ang);
//printf("p%.2f,%.2f,%.2f r%.2f,%.2f,%.2f lD%.2f,%.2f,%.2f\n",pos.x,pos.y,pos.z, rotate.x,rotate.y,rotate.z, lookDirection.x,lookDirection.y,lookDirection.z);

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
