#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "console.hpp"

float Camera::aspect = 4.f / 3.f;

Camera::Camera()
{
  pos = glm::vec3(0,1,4);
  lookAt = glm::vec3(0,0,0);
  lookDirection = glm::vec3(0,0,-1);
  lookUp = glm::vec3(0,1,0);
  fov = 60;
  nearP =   0.1;
  farP =  100.0;
  horizAngle = M_PI;
  vertAngle = 0.f;

  mode = CAM_ANGLES;

  update();
}

void Camera::yaw(float y) { horizAngle += y; }
void Camera::pitch(float p) { vertAngle += p; }
void Camera::forward(float dist) { pos += lookDirection * dist; }
void Camera::backward(float dist) { pos -= lookDirection * dist; }
void Camera::left(float dist) { pos -= lookRight * dist; }
void Camera::right(float dist) { pos += lookRight * dist; }

void Camera::look(glm::vec3 p, glm::vec3 l, glm::vec3 u)
{
  pos=p;
  lookAt=l;
  lookUp=u;
  mode=CAM_LOOK;
}

void Camera::update()
{
  if (mode == Camera::CAM_ANGLES) {
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    lookDirection = glm::vec3(
      cos(vertAngle) * sin(horizAngle),
      sin(vertAngle),
      cos(vertAngle) * cos(horizAngle)
    );

    // Right vector
    lookRight = glm::vec3(
      sin(horizAngle - 3.14f/2.0f),
      0,
      cos(horizAngle - 3.14f/2.0f)
    );

    // Up vector
    lookUp = glm::cross( lookRight, lookDirection );

    ProjectionMatrix = glm::perspective(fov, aspect, nearP, farP);
    ViewMatrix       = glm::lookAt(
      pos,           // Camera is here
      pos+lookDirection, // and looks here : at the same position, plus "direction"
      lookUp                  // Head is up (set to 0,-1,0 to look upside-down)
    );
  }
  else if (mode == Camera::CAM_LOOK) {
    ProjectionMatrix = glm::perspective(fov, aspect, nearP, farP);
    ViewMatrix       = glm::lookAt(pos, lookAt, lookUp);
  }
  else if (mode == Camera::CAM_LOOKDIRECTION) {
    ProjectionMatrix = glm::perspective(fov, aspect, nearP, farP);
    ViewMatrix       = glm::lookAt(pos, lookDirection + pos, lookUp);
  }

  VP = ProjectionMatrix * ViewMatrix;
}

void Camera::resize(const int w, const int h)
{
  if (h>0) aspect = float(w) / float(h);
  console.debugf("Camera resize %dx%d (aspect=%.3f)", w,h,aspect);
}
