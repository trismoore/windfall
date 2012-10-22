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

int QTree::qTreeNumberTotal=0, QTree::qTreeNumberVisible=0;

QTree::QTree(int startX, int startZ, int endX, int endZ, int maxSize, int level) 
	: level(level), startX(startX), startZ(startZ), endX(endX), endZ(endZ)
{
	++qTreeNumberTotal;

	if (!vbo) {
		vbo=new VBO();
		shader=new Shader("qtree.shader");
		vbo->pushData("vertex", 3, 8, vertexPositions);
		vbo->pushIndices(8*2, vertexIndices);
		vbo->create();
	}

//	console.logf("%d QTree(%d,%d - %d,%d max %d)",level, startX,startZ,endX,endZ,maxSize).indent();
	//lod=-1;
	visible = true;

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
		boundingBoxMin = children[0]->boundingBoxMin;
		boundingBoxMax = children[0]->boundingBoxMax;
		for (int i=1; i<4; ++i) {
			if (children[i]->boundingBoxMin.x < boundingBoxMin.x) boundingBoxMin.x = children[i]->boundingBoxMin.x;
			if (children[i]->boundingBoxMin.y < boundingBoxMin.y) boundingBoxMin.y = children[i]->boundingBoxMin.y;
			if (children[i]->boundingBoxMin.z < boundingBoxMin.z) boundingBoxMin.z = children[i]->boundingBoxMin.z;
			if (children[i]->boundingBoxMax.x > boundingBoxMax.x) boundingBoxMax.x = children[i]->boundingBoxMax.x;
			if (children[i]->boundingBoxMax.y > boundingBoxMax.y) boundingBoxMax.y = children[i]->boundingBoxMax.y;
			if (children[i]->boundingBoxMax.z > boundingBoxMax.z) boundingBoxMax.z = children[i]->boundingBoxMax.z;
		}			
	} else if (splitHoriz) {
		children[0] = new QTree(startX, startZ, midX, endZ, maxSize, level+1); // W
		children[1] = new QTree(  midX, startZ, endX, endZ, maxSize, level+1); // E
		boundingBoxMin = children[0]->boundingBoxMin;
		boundingBoxMax = children[0]->boundingBoxMax;
		int i=1; {
			if (children[i]->boundingBoxMin.x < boundingBoxMin.x) boundingBoxMin.x = children[i]->boundingBoxMin.x;
			if (children[i]->boundingBoxMin.y < boundingBoxMin.y) boundingBoxMin.y = children[i]->boundingBoxMin.y;
			if (children[i]->boundingBoxMin.z < boundingBoxMin.z) boundingBoxMin.z = children[i]->boundingBoxMin.z;
			if (children[i]->boundingBoxMax.x > boundingBoxMax.x) boundingBoxMax.x = children[i]->boundingBoxMax.x;
			if (children[i]->boundingBoxMax.y > boundingBoxMax.y) boundingBoxMax.y = children[i]->boundingBoxMax.y;
			if (children[i]->boundingBoxMax.z > boundingBoxMax.z) boundingBoxMax.z = children[i]->boundingBoxMax.z;
		}			
	} else if (splitVert) {
		children[0] = new QTree(startX, startZ, endX, midY, maxSize, level+1); // N
		children[2] = new QTree(startX,   midY, endX, endZ, maxSize, level+1); // S
		boundingBoxMin = children[0]->boundingBoxMin;
		boundingBoxMax = children[0]->boundingBoxMax;
		int i=2; {
			if (children[i]->boundingBoxMin.x < boundingBoxMin.x) boundingBoxMin.x = children[i]->boundingBoxMin.x;
			if (children[i]->boundingBoxMin.y < boundingBoxMin.y) boundingBoxMin.y = children[i]->boundingBoxMin.y;
			if (children[i]->boundingBoxMin.z < boundingBoxMin.z) boundingBoxMin.z = children[i]->boundingBoxMin.z;
			if (children[i]->boundingBoxMax.x > boundingBoxMax.x) boundingBoxMax.x = children[i]->boundingBoxMax.x;
			if (children[i]->boundingBoxMax.y > boundingBoxMax.y) boundingBoxMax.y = children[i]->boundingBoxMax.y;
			if (children[i]->boundingBoxMax.z > boundingBoxMax.z) boundingBoxMax.z = children[i]->boundingBoxMax.z;
		}			
	} else {
//		console.debugf("%d,%d - %d,%d, %d", startX,startZ, endX,endZ, level);
		landPatch = new LandPatch(startX, startZ);
		boundingBoxMin = landPatch->boundingBoxMin;
		boundingBoxMax = landPatch->boundingBoxMax;
	}

	centre = 0.5f * (boundingBoxMin + boundingBoxMax);
	radius = glm::distance(centre, boundingBoxMin);
//	console.debugf("%d,%d - %d,%d, %d centre:%.3f,%.3f,%.3f, rad %.3f", startX,startZ, endX,endZ, level, centre.x,centre.y,centre.z, radius);

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
	vbo->bind(shader);

	qTreeNumberVisible=0;

	glDepthFunc(GL_ALWAYS);
	debugRenderRecurse();
	glDepthFunc(GL_LESS);

printf("QTree::rendered %d / %d\n", qTreeNumberVisible, qTreeNumberTotal);
}

void QTree::debugRenderRecurse()
{
	if (!visible) return;

	++qTreeNumberVisible;

	if (landPatch)  // only display leaves
	{
		shader->set3f("bbMin", boundingBoxMin);
		shader->set3f("bbMax", boundingBoxMax);

		float distance = glm::distance(centre, g_camera->pos);
		int l;
		if (distance < 0.8) l=0;       // slightly favour close distances.  we're within this patch's sphere at 0.707 or there abouts
		else if (distance < 1.2) l=1;
		else l = min(LandPatch::numberOfLODLevels-1, int(distance*distance*1.4));

		shader->set3f("colour",drawColours[l]);

		vbo->drawElements(GL_LINES, 8*2, 0);
	}

//	if (level <= 4) // only draw first few levels
	{
		for (int i=0; i<4; ++i)
			if (children[i]) 
				children[i]->debugRenderRecurse();	
	}
}

void QTree::render()
{
	if (!visible) return;
	if (landPatch) landPatch->draw();
	else {
		for (int i=0; i<4; ++i)
			if (children[i])
				children[i]->render();
	}
}

void QTree::calculateLOD(Camera* camera)
{
	visible = camera->isSphereVisible(centre,radius);
//	lod = camera->getLOD(centre, radius);
//printf("%.3f,%.3f %.3f = %.3f\n", centre.x,centre.z, radius, lod);
	if (visible) { //lod>=0) {
		for (int i=0; i<4; ++i)
			if (children[i])
				children[i]->calculateLOD(camera);
	}
}

