#ifndef QTREE_HPP
#define QTREE_HPP

#include "glm/glm.hpp"

class VBO;
class Shader;
class LandPatch;
class Camera;

class QTree {
  QTree *children[4];
  LandPatch * landPatch;

//  float lod; // 0=at near clip plane, 1=at far, <0 = not visible
  float distance;
  bool visible;

  int level; // 0==root
  int startX,startZ,endX,endZ,sizeX,sizeZ;
  glm::vec3 centre;
  glm::vec3 boundingBoxMin, boundingBoxMax;
  float radius;

  static const float drawColours[10][3];   // 10x rgb
  static const float vertexPositions[8*3]; //  8x vec3
  static const unsigned int vertexIndices[8*2];   //  8x lines

  // debugging
  static VBO *vbo;
  static Shader *shader;
  static int qTreeNumberTotal, qTreeNumberVisible;

public:

  QTree(int startX, int startZ, int endX, int endZ, int maxSize, int level);
  ~QTree();

  void calculateCentreAndRadius();

  void calculateLOD(Camera* camera);

  void debugRenderRecurse(); // the actual drawing
  void debugRender(); // draw cubes to show tree (setup then recurse)
  void render();
};

#endif//QTREE_HPP
