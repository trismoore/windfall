#ifndef QTREE_HPP
#define QTREE_HPP

class VBO;
class Shader;
class LandPatch;

class QTree {
  QTree *children[4];
  LandPatch * landPatch;

  float lod; // 0=at near clip plane, 1=at far, <0 = not visible
  int level; // 0==root
  int startX,startZ,endX,endZ,sizeX,sizeZ;

  static const float drawColours[10][3];   // 10x rgb
  static const float vertexPositions[8*3]; //  8x vec3
  static const unsigned int vertexIndices[8*2];   //  8x lines

  // debugging
  static VBO *vbo;
  static Shader *shader;

public:

  QTree(int startX, int startZ, int endX, int endZ, int maxSize, int level);
  ~QTree();

  void debugRender(); // draw cubes to show tree
  void render();
};

#endif//QTREE_HPP
