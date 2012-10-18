#include "qtree.hpp"
#include "console.hpp"

#include "vbo.hpp"
#include "shader.hpp"
#include "camera.hpp"

#include "landpatch.hpp"

extern Camera* g_camera;

int nextPowerOfTwo(int n)
{
	int p = 0;
	while ((1<<p) < n) ++p;
	return (1<<p);
}

const float QTree::drawColours[10][3] = { 1,1,1, 1,0,0, 0,1,0, 0,0,1, 1,1,0, 0,1,1, 1,0,1, 0.5,1,0.3, 0.7,0.1,0.9, 0.9,0.6,0.7 };
const float QTree::vertexPositions[8*3] = { 0,0,0, 1,0,0, 0,0,1, 1,0,1, 0,1,0, 1,1,0, 0,1,1, 1,1,1 };
const unsigned int QTree::vertexIndices[8*2] = { 0,4, 1,5, 2,6, 3,7, 4,5, 4,6, 5,7, 6,7 };
VBO* QTree::vbo =0;
Shader* QTree::shader =0;

QTree::QTree(int startX, int startZ, int endX, int endZ, int maxSize, int level) 
	: level(level), startX(startX), startZ(startZ), endX(endX), endZ(endZ)
{
	if (!vbo) {
		vbo=new VBO();
		shader=new Shader("qtree.shader");
		vbo->pushData("vertex", 3, 8, vertexPositions);
		vbo->pushIndices(8*2, vertexIndices);
		vbo->create();
	}

//	console.logf("%d QTree(%d,%d - %d,%d max %d)",level, startX,startZ,endX,endZ,maxSize).indent();
	lod=-1;

	sizeX = endX-startX, sizeZ = endZ-startZ;

	int midX = startX + (sizeX+1) / 2; // round up
	int midY = startZ + (sizeZ+1) / 2;

	// it is possible to just split in half, but it's probably more efficient to get to a square as soon as possible
	midX = startX + nextPowerOfTwo(midX - startX);
	midY = startZ + nextPowerOfTwo(midY - startZ);

	bool splitHoriz=false, splitVert=false;
	
	if (endX - startX > maxSize) splitHoriz=true;
	if (endZ - startZ > maxSize) splitVert =true;

	// 4 cases: no split (we are leaf), split horiz, split vert, split both
	for (int i=0; i<4; ++i) children[i]=0;
	landPatch=0;

	if (splitHoriz && splitVert) {
		children[0] = new QTree(startX, startZ, midX, midY, maxSize, level+1); // NW
		children[1] = new QTree(  midX, startZ, endX, midY, maxSize, level+1); // NE
		children[2] = new QTree(startX,   midY, midX, endZ, maxSize, level+1); // SW
		children[3] = new QTree(  midX,   midY, endX, endZ, maxSize, level+1); // SE
	} else if (splitHoriz) {
		children[0] = new QTree(startX, startZ, midX, endZ, maxSize, level+1); // W
		children[1] = new QTree(  midX, startZ, endX, endZ, maxSize, level+1); // E
	} else if (splitVert) {
		children[0] = new QTree(startX, startZ, endX, midY, maxSize, level+1); // N
		children[2] = new QTree(startX,   midY, endX, endZ, maxSize, level+1); // S
	} else {
//		console.debugf("%d,%d - %d,%d, %d", startX,startZ, endX,endZ, level);
		landPatch = new LandPatch(startX, 0, startZ);
	}

//	console.outdent();
}

QTree::~QTree()
{
	for (int i=0; i<4; ++i) delete children[i];
	delete landPatch;
}

void QTree::debugRender()
{
	shader->setCamera(g_camera);

	shader->setf("startX",startX); shader->setf("startZ",startZ);
	shader->setf("sizeX",sizeX); shader->setf("sizeZ",sizeZ);
	shader->set3f("colour",drawColours[level]);

	vbo->bind(shader);
	vbo->drawElements(GL_LINES, 8*2, 0);

	if (level <= 4) // only draw first few levels
	{
		for (int i=0; i<4; ++i)
			if (children[i]) 
				children[i]->debugRender();	
	}
}

void QTree::render()
{
	if (landPatch) landPatch->draw();
	else {
		for (int i=0; i<4; ++i)
			if (children[i])
				children[i]->render();
	}
}
