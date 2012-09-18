#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera
{
  glm::vec3 pos, lookAt, lookDirection, lookRight, lookUp;
  float fov;
  float nearP, farP;
  float horizAngle, vertAngle;

  enum { CAM_LOOK, CAM_LOOKDIRECTION, CAM_ANGLES } mode;

public:
  glm::mat4 ViewMatrix;
  glm::mat4 ProjectionMatrix;
  glm::mat4 VP; // = Projection*View

  static float aspect;

  Camera();

  void setPos(glm::vec3 p);

  void look(glm::vec3 p, glm::vec3 l, glm::vec3 u=glm::vec3(0,1,0));

  void yaw(float y);
  void pitch(float p);
  void forward(float dist_per_second);
  void backward(float dist_per_second);
  void left(float dist_per_second);
  void right(float dist_per_second);

  void update();

  static void resize(const int w, const int h) { if (h>0) aspect = float(w) / float(h); }
};

#endif//CAMERA_H
